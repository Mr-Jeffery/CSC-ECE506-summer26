#!/bin/bash
# Report experiment 2 — ASSOCIATIVITY sweep (size 1024KB, block 64B), all protocols.
# The fully-associative config (16384 ways) uses a linear tag search and is far slower
# than the others. It is tractable on streamcluster (~15 min) but impractical on the
# 8.9 GB swaptions trace, so swaptions is capped at 8-way.
# Reference answers: results/reference/<trace>/assoc.csv
cd "$(dirname "$0")"; mkdir -p results/q2

# streamcluster: full sweep including fully-associative
TR=traces/streamcluster_simlarge_cleaned
if [ -f "$TR" ]; then
    echo "associativity sweep (1/2/4/8/full) on streamcluster ..."
    ./simulate_cache configs/assoc.txt "$TR" > results/q2/streamcluster_assoc.csv
else
    echo "skip: $TR not found"
fi

# swaptions: 1/2/4/8-way only (drop the fully-associative lines)
TR=traces/swaptions_medium_cleaned
if [ -f "$TR" ]; then
    echo "associativity sweep (1/2/4/8) on swaptions ..."
    cap=$(mktemp)
    grep -vE ' 16384 ' configs/assoc.txt > "$cap"
    ./simulate_cache "$cap" "$TR" > results/q2/swaptions_assoc.csv
    rm -f "$cap"
else
    echo "skip: $TR not found"
fi
