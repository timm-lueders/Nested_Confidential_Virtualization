#
#  Copyright (c) 2011-2015, ARM Limited. All rights reserved.
#  Copyright (c) 2014-2016, Linaro Limited. All rights reserved.
#  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

################################################################################
#
# FV Section
#
# [FV] section is used to define what components or modules are placed within a flash
# device file.  This section also defines order the components and modules are positioned
# within the image.  The [FV] section consists of define statements, set statements and
# module statements.
#
################################################################################

[FV.FvMain]
FvNameGuid         = 64074afe-340a-4be6-94ba-91b5b4d0f71e
BlockSize          = 0x40
NumBlocks          = 0         # This FV gets compressed so make it just big enough
FvAlignment        = 16        # FV alignment and FV attributes setting.
ERASE_POLARITY     = 1
MEMORY_MAPPED      = TRUE
STICKY_WRITE       = TRUE
LOCK_CAP           = TRUE
LOCK_STATUS        = TRUE
WRITE_DISABLED_CAP = TRUE
WRITE_ENABLED_CAP  = TRUE
WRITE_STATUS       = TRUE
WRITE_LOCK_CAP     = TRUE
WRITE_LOCK_STATUS  = TRUE
READ_DISABLED_CAP  = TRUE
READ_ENABLED_CAP   = TRUE
READ_STATUS        = TRUE
READ_LOCK_CAP      = TRUE
READ_LOCK_STATUS   = TRUE

  INF MdeModulePkg/Core/Dxe/DxeMain.inf
  INF MdeModulePkg/Universal/PCD/Dxe/Pcd.inf
  INF OvmfPkg/Fdt/VirtioFdtDxe/VirtioFdtDxe.inf
  INF EmbeddedPkg/Drivers/FdtClientDxe/FdtClientDxe.inf
  INF OvmfPkg/Fdt/HighMemDxe/HighMemDxe.inf

  #
  # PI DXE Drivers producing Architectural Protocols (EFI Services)
  #
  INF ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  INF MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  INF MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf
  INF MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  INF MdeModulePkg/Universal/FaultTolerantWriteDxe/FaultTolerantWriteDxe.inf
  INF MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf
!if $(SECURE_BOOT_ENABLE) == TRUE
  INF SecurityPkg/VariableAuthenticated/SecureBootConfigDxe/SecureBootConfigDxe.inf
!endif
  INF MdeModulePkg/Universal/MonotonicCounterRuntimeDxe/MonotonicCounterRuntimeDxe.inf
  INF MdeModulePkg/Universal/ResetSystemRuntimeDxe/ResetSystemRuntimeDxe.inf
  INF EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  INF EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf
  INF MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf

  #
  # Multiple Console IO support
  #
  INF MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  INF MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  INF MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  INF MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf
  INF MdeModulePkg/Universal/SerialDxe/SerialDxe.inf

  INF ArmPkg/Drivers/ArmGicDxe/ArmGicDxe.inf
  INF ArmPkg/Drivers/TimerDxe/TimerDxe.inf
  INF OvmfPkg/VirtNorFlashDxe/VirtNorFlashDxe.inf
  INF MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  INF SecurityPkg/RandomNumberGenerator/RngDxe/RngDxe.inf

  #
  # FAT filesystem + GPT/MBR partitioning + UDF filesystem + virtio-fs
  #
  INF MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  INF MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  INF FatPkg/EnhancedFatDxe/Fat.inf
  INF MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  INF MdeModulePkg/Universal/Disk/UdfDxe/UdfDxe.inf
  INF OvmfPkg/VirtioFsDxe/VirtioFsDxe.inf

  #
  # Status Code Routing
  #
  INF MdeModulePkg/Universal/ReportStatusCodeRouter/RuntimeDxe/ReportStatusCodeRouterRuntimeDxe.inf

  #
  # Platform Driver
  #
  INF OvmfPkg/VirtioBlkDxe/VirtioBlk.inf
  INF OvmfPkg/VirtioNetDxe/VirtioNet.inf
  INF OvmfPkg/VirtioScsiDxe/VirtioScsi.inf
  INF OvmfPkg/VirtioRngDxe/VirtioRng.inf
  INF OvmfPkg/VirtioSerialDxe/VirtioSerial.inf

!include OvmfPkg/Include/Fdf/ShellDxe.fdf.inc

  #
  # Bds
  #
  INF MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  INF MdeModulePkg/Universal/DisplayEngineDxe/DisplayEngineDxe.inf
  INF MdeModulePkg/Universal/SetupBrowserDxe/SetupBrowserDxe.inf
  INF MdeModulePkg/Universal/DriverHealthManagerDxe/DriverHealthManagerDxe.inf
  INF MdeModulePkg/Universal/BdsDxe/BdsDxe.inf
  INF MdeModulePkg/Application/UiApp/UiApp.inf
  INF OvmfPkg/QemuKernelLoaderFsDxe/QemuKernelLoaderFsDxe.inf

  #
  # Networking stack
  #
!include NetworkPkg/Network.fdf.inc


  #
  # SCSI Bus and Disk Driver
  #
  INF MdeModulePkg/Bus/Scsi/ScsiBusDxe/ScsiBusDxe.inf
  INF MdeModulePkg/Bus/Scsi/ScsiDiskDxe/ScsiDiskDxe.inf

  #
  # NVME Driver
  #
  INF MdeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf

  #
  # SMBIOS Support
  #
  INF MdeModulePkg/Universal/SmbiosDxe/SmbiosDxe.inf
  INF OvmfPkg/SmbiosPlatformDxe/SmbiosPlatformDxe.inf

  #
  # ACPI Support
  #
  INF OvmfPkg/PlatformHasAcpiDtDxe/PlatformHasAcpiDtDxe.inf
!if $(ARCH) == AARCH64
  INF MdeModulePkg/Universal/Acpi/AcpiTableDxe/AcpiTableDxe.inf
  INF MdeModulePkg/Universal/Acpi/BootGraphicsResourceTableDxe/BootGraphicsResourceTableDxe.inf
  INF OvmfPkg/AcpiPlatformDxe/AcpiPlatformDxe.inf
!endif

  #
  # PCI support
  #
  INF UefiCpuPkg/CpuMmio2Dxe/CpuMmio2Dxe.inf
  INF MdeModulePkg/Bus/Pci/PciHostBridgeDxe/PciHostBridgeDxe.inf
  INF MdeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  INF OvmfPkg/PciHotPlugInitDxe/PciHotPlugInit.inf
  INF OvmfPkg/VirtioPciDeviceDxe/VirtioPciDeviceDxe.inf
  INF OvmfPkg/Virtio10Dxe/Virtio10.inf

  #
  # Video support
  #
  INF OvmfPkg/QemuRamfbDxe/QemuRamfbDxe.inf
  INF OvmfPkg/VirtioGpuDxe/VirtioGpu.inf
  INF OvmfPkg/PlatformDxe/Platform.inf

  #
  # USB Support
  #
  INF MdeModulePkg/Bus/Pci/UhciDxe/UhciDxe.inf
  INF MdeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
  INF MdeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
  INF MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
  INF MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
  INF MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf

  #
  # Hash2 Protocol producer
  #
  INF SecurityPkg/Hash2DxeCrypto/Hash2DxeCrypto.inf

  #
  # TPM2 support
  #
!if $(TPM2_ENABLE) == TRUE
  INF SecurityPkg/Tcg/Tcg2Dxe/Tcg2Dxe.inf
!if $(TPM2_CONFIG_ENABLE) == TRUE
  INF SecurityPkg/Tcg/Tcg2Config/Tcg2ConfigDxe.inf
!endif
!endif

  #
  # TianoCore logo (splash screen)
  #
  INF MdeModulePkg/Logo/LogoDxe.inf

  #
  # Ramdisk support
  #
  INF MdeModulePkg/Universal/Disk/RamDiskDxe/RamDiskDxe.inf
