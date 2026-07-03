#!/bin/bash
# Regenerate the reference-solution result matrix with simulate_cache_ref.
# Ordered cheapest-first so files land progressively. Fully-associative (16384-way)
# configs are the slow tail (linear tag search), especially on swaptions.
set -e
cd "$(dirname "$0")"
REF=./simulate_cache_ref
SC=traces/streamcluster_simlarge_cleaned
SW=traces/swaptions_medium_cleaned
mkdir -p results/reference/streamcluster results/reference/swaptions
log() { echo "[$(date +%H:%M:%S)] $*"; }

log "streamcluster size/block..."
$REF configs/size.txt  "$SC" > results/reference/streamcluster/size.csv
$REF configs/block.txt "$SC" > results/reference/streamcluster/block.csv

log "swaptions size/block..."
$REF configs/size.txt  "$SW" > results/reference/swaptions/size.csv
$REF configs/block.txt "$SW" > results/reference/swaptions/block.csv

log "streamcluster assoc (incl. fully-associative, ~15 min)..."
$REF configs/assoc.txt "$SC" > results/reference/streamcluster/assoc.csv

# Fully-associative (16384-way) is impractically slow on the 8.9 GB swaptions trace
# (hours), so the swaptions associativity sweep is capped at 8-way.
log "swaptions assoc (1/2/4/8-way; fully-associative skipped)..."
cap=$(mktemp)
grep -vE ' 16384 ' configs/assoc.txt > "$cap"
$REF "$cap" "$SW" > results/reference/swaptions/assoc.csv
rm -f "$cap"

log "ALL REFERENCE RESULTS DONE"
