#!/bin/bash
# Check your Dragon implementation against the reference solution.
# Runs a few configs with YOUR simulate_cache and diffs against the pre-generated
# reference output in results/reference/test/. No differences => your Dragon matches.

mkdir -p results/test
TRACE=traces/streamcluster_simlarge_cleaned

if [ ! -f "$TRACE" ]; then
    echo "Trace file not found: $TRACE (copy it into traces/ first)."
    exit 1
fi

for B in 32 64 128; do
    ./simulate_cache 1024k $B 4 dragon lru Broadcast "$TRACE" > results/test/dragon_${B}.txt
    echo "Differences for dragon_${B} (1024k, ${B}B blocks, 4-way):"
    if diff results/test/dragon_${B}.txt results/reference/test/dragon_${B}.txt; then
        echo "  match"
    fi
    echo ""
done
