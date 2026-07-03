# Results

Reference outputs produced by `simulate_cache_ref` (the complete solution). Your own
`simulate_cache` should reproduce these once MESI and Dragon are implemented.

## Layout

```
results/
├── reference/
│   ├── streamcluster/     # full report matrix (answer key), streamcluster trace
│   │   ├── size.csv       #   cache-size sweep   (assoc 4, block 64B)
│   │   ├── assoc.csv      #   associativity sweep (size 1024KB, block 64B)
│   │   └── block.csv      #   block-size sweep   (size 2048KB, assoc 2)
│   ├── swaptions/         # same three sweeps, swaptions trace
│   └── test/              # small per-config outputs the test_*.sh scripts diff against
├── q1/  q2/  q3/          # YOUR outputs, written by q1.sh / q2.sh / q3.sh
└── test/                  # YOUR outputs, written by test_mesi.sh / test_dragon.sh
```

## Config id ↔ parameters

The sweep CSVs are produced in **batch mode**, so the first column (`config`) is a numeric
id, and rows may appear in any order. `configs/legend.csv` maps each `config` id (within a
sweep) to its `cache_size / block_size / assoc / protocol`. To read a sweep, join on
`config` (and sort by `config`,`core` for a stable order).

## Regenerating

```
./gen_reference.sh          # regenerates results/reference/{streamcluster,swaptions}/*
bash q4.sh                  # regenerates YOUR results/q1..q3 with ./simulate_cache
```

Note: the fully-associative associativity config (16384 ways) uses a linear tag search and
is slow. It is run on **streamcluster only** (`assoc.csv` there has config ids 1–15); the
swaptions associativity sweep is capped at 8-way (`swaptions/assoc.csv` has ids 1–12). All
other configs finish in seconds/minutes.
