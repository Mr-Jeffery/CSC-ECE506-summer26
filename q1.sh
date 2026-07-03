#!/bin/bash
# Report experiment 1 — cache SIZE sweep (associativity 4, block 64B), all protocols.
# Runs YOUR simulate_cache in batch mode (one pass per trace). See configs/legend.csv
# for the config_id -> parameters mapping. Reference answers: results/reference/<trace>/size.csv
cd "$(dirname "$0")"; mkdir -p results/q1
for T in streamcluster_simlarge_cleaned swaptions_medium_cleaned; do
    TR=traces/$T; short=${T%%_*}
    [ -f "$TR" ] || { echo "skip: $TR not found"; continue; }
    echo "size sweep on $short ..."
    ./simulate_cache configs/size.txt "$TR" > results/q1/${short}_size.csv
done
