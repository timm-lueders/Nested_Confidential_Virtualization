// SPDX-License-Identifier: MIT OR Apache-2.0 Copyright (c) Microsoft Corporation
// Author: Jon Lange (jlange@microsoft.com)

use crate::cpu::idt::svsm::common_isr_handler;
use crate::cpu::irq_state::{raw_get_tpr, tpr_from_vector};
use crate::cpu::percpu::this_cpu;
use crate::cpu::IrqState;
use crate::error::SvsmError;
use crate::mm::page_visibility::SharedBox;
use crate::mm::virt_to_phys;
use crate::sev::ghcb::GHCB;

use bitfield_struct::bitfield;
use core::arch::asm;
use core::sync::atomic::{AtomicU32, AtomicU8, Ordering};
use zerocopy::FromBytes;

#[bitfield(u8)]
pub struct HVDoorbellFlags {
    pub nmi_pending: bool,
    pub mc_pending: bool,
    #[bits(5)]
    rsvd_6_2: u8,
    pub no_further_signal: bool,
}

#[bitfield(u32)]
pub struct HVExtIntStatus {
    pub pending_vector: u8,
    pub nmi_pending: bool,
    pub mc_pending: bool,
    pub level_sensitive: bool,
    #[bits(3)]
    rsvd_13_11: u32,
    pub multiple_vectors: bool,
    #[bits(12)]
    rsvd_26_15: u32,
    ipi_requested: bool,
    #[bits(3)]
    rsvd_30_28: u32,
    pub vector_31: bool,
}

#[repr(C)]
#[derive(Debug, FromBytes)]
pub struct HVExtIntInfo {
    pub status: AtomicU32,
    pub irr: [AtomicU32; 7],
    pub isr: [AtomicU32; 8],
}

/// Allocates a new HV doorbell page and registers it on the hypervisor
/// using the given GHCB.
pub fn allocate_hv_doorbell_page(ghcb: &GHCB) -> Result<SharedBox<HVDoorbell>, SvsmError> {
    let page = SharedBox::<HVDoorbell>::try_new_zeroed()?;

    let vaddr = page.addr();
    let paddr = virt_to_phys(vaddr);
    ghcb.register_hv_doorbell(paddr)?;

    Ok(page)
}

#[repr(C)]
#[derive(Debug, FromBytes)]
pub struct HVDoorbell {
    pub vector: AtomicU8,
    pub flags: AtomicU8,
    pub no_eoi_required: AtomicU8,
    pub per_vmpl_events: AtomicU8,
    reserved_63_4: [AtomicU8; 60],
    pub per_vmpl: [HVExtIntInfo; 3],
}

impl HVDoorbell {
    pub fn process_pending_events(&self) {
        // Clear the NoFurtherSignal bit before processing.  If any additional
        // signal comes in after processing has commenced, it may be missed by
        // this loop, but it will be detected when interrupts are processed
        // again.  Also clear the NMI bit, since NMIs are not expected.
        let no_further_signal_mask: u8 = HVDoorbellFlags::new()
            .with_no_further_signal(true)
            .with_nmi_pending(true)
            .into();
        let flags = HVDoorbellFlags::from(
            self.flags
                .fetch_and(!no_further_signal_mask, Ordering::Relaxed),
        );

        // #MC handling is not possible, so panic if a machine check has
        // occurred.
        if flags.mc_pending() {
            panic!("#MC exception delivered via #HV");
        }

        // Consume interrupts as long as they are available and as long as
        // they are appropriate for the current task priority.
        let tpr = raw_get_tpr();
        let mut vector = self.vector.load(Ordering::Relaxed);
        loop {
            // Check whether an interrupt is present.  If it is at or below
            // the current task priority, then it will not be dispatched.
            // If the interrupt is not dispatched, then the vector must remain
            // in the #HV doorbell page so the hypervisor knows it has not been
            // placed into service.
            if (tpr_from_vector(vector as usize)) <= tpr {
                break;
            }
            match self
                .vector
                .compare_exchange_weak(vector, 0, Ordering::Relaxed, Ordering::Relaxed)
            {
                Ok(_) => common_isr_handler(vector as usize),
                Err(new) => vector = new,
            }
        }

        // Ignore per-VMPL events; these will be consumed when APIC emulation
        // is performed.
    }

    fn events_to_process(&self) -> bool {
        // If NoFurtherSignal is set, it means the hypervisor has posted a
        // new event, and there must be an event that requires processing.  If
        // NoFurtherSignal is not set, then processing is required if the
        // pending vector is a higher priority than the current TPR.
        let flags = HVDoorbellFlags::from(self.flags.load(Ordering::Relaxed));
        if flags.no_further_signal() {
            true
        } else {
            let min_vector = (raw_get_tpr() + 1) << 4;
            self.vector.load(Ordering::Relaxed) as usize >= min_vector
        }
    }

    /// This function must always be called with interrupts enabled.
    pub fn process_if_required(&self, irq_state: &IrqState) {
        while self.events_to_process() {
            // #HV event processing must always be performed with interrupts
            // disabled.
            irq_state.disable();
            self.process_pending_events();

            // Do not call the standard enable routine, because that could
            // recursively call to process new events, resulting in unbounded
            // nesting.  Intead, decrement the nesting count and directly
            // enable interrupts here before continuing the loop to see
            // whether additional events have arrived.
            assert_eq!(irq_state.pop_nesting(), 0);
            // SAFETY: interrupts can now be enabled directly.
            unsafe {
                asm!("sti");
            }
        }
    }

    pub fn no_eoi_required(&self) -> bool {
        // Check to see if the "no EOI required" flag is set to determine
        // whether an explicit EOI can be avoided.
        let mut no_eoi_required = self.no_eoi_required.load(Ordering::Relaxed);
        loop {
            // If the flag is not set, then an explicit EOI is required.
            if (no_eoi_required & 1) == 0 {
                return false;
            }
            // Attempt to atomically clear the flag.
            match self.no_eoi_required.compare_exchange_weak(
                no_eoi_required,
                no_eoi_required & !1,
                Ordering::Relaxed,
                Ordering::Relaxed,
            ) {
                Ok(_) => break,
                Err(new) => no_eoi_required = new,
            }
        }

        // If the flag was successfully cleared, then no explicit EOI is
        // required.
        true
    }
}

/// Gets the HV doorbell page configured for this CPU.
///
/// # Panics
///
/// Panics if te HV doorbell page has not been set up beforehand.
pub fn current_hv_doorbell() -> &'static HVDoorbell {
    this_cpu()
        .hv_doorbell()
        .expect("HV doorbell page dereferenced before allocating")
}

/// # Safety
/// This function takes a raw pointer to the #HV doorbell page because it is
/// called directly from assembly, and should not be invoked directly from
/// Rust code.
#[no_mangle]
pub unsafe extern "C" fn process_hv_events(hv_doorbell: *const HVDoorbell) {
    // Update the IRQ nesting state of the current CPU so calls to common
    // code recognize that interrupts have been disabled.  Proceed as if
    // interrupts were previously enabled, so that any code that deals with
    // maskable interrupts knows that interrupts were enabled prior to reaching
    // this point.
    let cpu = this_cpu();
    cpu.irqs_push_nesting(true);
    // SAFETY: the correctness of #HV doorbell page has been guaranteed by the
    // caller.
    unsafe {
        (*hv_doorbell).process_pending_events();
    }
    cpu.irqs_pop_nesting();
}
