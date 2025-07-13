#!/bin/bash

MODE=$1  # sev_on oder sev_off
ITERATIONS=20

CPU_OUT=~/benchmark/${MODE}/cpu
RAM_OUT=~/benchmark/${MODE}/ram

mkdir -p ${CPU_OUT} ${RAM_OUT}

echo "Starte Benchmarks für Modus: ${MODE}"

# CPU-Benchmarks mit sysbench
for i in $(seq 1 ${ITERATIONS}); do
    echo "CPU Benchmark ${i}/${ITERATIONS}"
    sysbench cpu --threads=8 --cpu-max-prime=20000 run > ${CPU_OUT}/sysbench_cpu_${i}.log
done

# RAM-Benchmarks mit sysbench
for i in $(seq 1 ${ITERATIONS}); do
    echo "RAM Benchmark ${i}/${ITERATIONS}"
    sysbench memory --memory-total-size=10G run > ${RAM_OUT}/sysbench_ram_${i}.log
done

# Phoronix CPU-Tests korrekt
phoronix-test-suite batch-benchmark compress-7zip openssl c-ray --iterations 5 --results-name phoronix-cpu-${MODE}

# Verschiebe Phoronix-Ergebnisse in CPU-Verzeichnis
mv ~/.phoronix-test-suite/test-results/phoronix-cpu-${MODE} ${CPU_OUT}/phoronix-cpu-${MODE}

# Phoronix RAM-Tests korrekt
phoronix-test-suite batch-b