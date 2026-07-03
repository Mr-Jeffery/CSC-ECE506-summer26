#!/bin/bash
# Run all three report experiments (size, associativity, block size) on both traces.
cd "$(dirname "$0")"
bash q1.sh
bash q2.sh
bash q3.sh
echo "All experiments done. Results under results/q1, results/q2, results/q3."
