#!/bin/bash
# Report experiment 3 — cache-BLOCK-size sweep (size 2048KB, associativity 2), all protocols.
# Runs YOUR simulate_cache in batch mode. Reference answers: results/reference/<trace>/block.csv
cd "$(dirname "$0")"; mkdir -p results/q3
for T in streamcluster_simlarge_cleaned swaptions_medium_cleaned; do
    TR=traces/$T; short=${T%%_*}
    [ -f "$TR" ] || { echo "skip: $TR not found"; continue; }
    echo "block-size sweep on $short ..."
    ./simulate_cache configs/block.txt "$TR" > results/q3/${short}_block.csv
done
