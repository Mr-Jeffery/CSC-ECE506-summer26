# CSC/ECE 506: Architecture of Parallel Computers
## Program 3: Bus-Based Cache Coherence Protocols
**Due: Friday, July 17, 2026**

---

## 1. Problem Description

This project asks you to add new features to a trace-driven cache-coherence simulator. It is intended to give you an idea of how parallel architectures handle coherence, and how to interpret performance data.

You are given a C++ cache simulator implementing the **MSI** protocol, and you need to extend that simulator to implement the **MESI** and **Dragon** protocols. Together, these three protocols span the two fundamental design points of bus-based coherence — invalidation-based (MSI, MESI) and update-based (Dragon) — and let you isolate the performance contribution of the *Exclusive* state.

**MESI** is an invalidation-based protocol that augments MSI with the **Exclusive (E)** state. When a processor reads a block that no other cache currently holds, the block is brought in as E rather than S. A subsequent write upgrades E→M silently, with no bus transaction. This eliminates one `BusRdX` (or `BusUpgr`) per first-write-after-read sequence on private data, which is a common pattern.

**Dragon** is an update-based protocol with four states: **Exclusive (E)**, **Modified (M)**, **Shared-Clean (Sc)**, and **Shared-Modified (Sm)**. Writes to lines in Sc or Sm broadcast a `BusUpd` (write-update) message rather than invalidating other copies. Exactly one cache holds a shared line in Sm and is the *owner* responsible for writing the block back to memory on eviction. Dragon has no Invalid state for resident lines (a miss simply allocates a new line) and never issues `BusRdX` or `BusUpgr`.

Your project should be built on a Linux machine. The most challenging part of this machine problem is to understand how caches and coherence protocols are implemented. Once you understand this, the rest of the assignment is straightforward.

---

## 2. Simulator

### 2.1 How to build the simulator

You are provided with a working C++ program for a cache implementing the MSI protocol. Coherence protocols are separated from the cache machinery by inheritance:

- `src/coherence/coherence_protocol.h` declares the abstract base class **`CoherenceProtocol`**. Each protocol is a subclass that overrides the methods below.
- `src/cache.cc` / `src/cache.h` implement the **`Cache`** class — everything common to every protocol (set/way lookup, allocation, eviction, statistics, and driving the bus).
- `src/coherence/msi.cc` implements **MSI** and is complete; use it as your worked example.
- `src/coherence/mesi.cc` and `src/coherence/dragon.cc` are **stubs for you to implement**.

A `CoherenceProtocol` subclass overrides the methods that differ from protocol to protocol:

- `void PrRd(cache_line *line)`
- `void PrWr(cache_line *line)`
- `bool BusRd(cache_line *line)`
- `bool BusRdX(cache_line *line)`
- `bool BusUpgr(cache_line *line)`
- `bool BusUpdt(cache_line *line)`
- `bool isWriteBackNeeded(state_e state)`
- `bool doesDirtySharing()`

Note that some protocols do not implement some of these methods (for example, MSI and MESI never handle `BusUpdt`, and Dragon never handles `BusRdX` or `BusUpgr`). Any method a protocol does not override falls back to the base-class default. The `Bus*` methods take care of snooping *one* bus operation in *a single* cache, and return `true` if that cache flushed the line onto the bus.

The `PrRd`, `PrWr`, and `Bus*` methods are different for each protocol, because each protocol handles processor and bus actions differently. The `isWriteBackNeeded` method is different because when a line is evicted it must be written back only if it holds dirty data, and the states that represent dirty data differ across protocols. In particular, *Dragon writes back from both M and Sm* — Sm is dirty even though the line is shared — so `Dragon::isWriteBackNeeded` must return `true` for both. `doesDirtySharing` reports whether a protocol lets a cache supply dirty data to a peer without a simultaneous memory writeback (true for Dragon's Sm owner, false for MSI/MESI).

**How a protocol drives the bus.** When `PrRd` or `PrWr` needs to broadcast, it calls

```cpp
bool copies_exist = cache.issueBusMsg(bus_msg);   // bus_msg is BusRead, BusReadX, BusUpdate, ...
```

`Cache::issueBusMsg` (in `src/cache.cc`) hands the message to the **directory** (the `Broadcast` directory in `src/directory/broadcast.cc`), which delivers it to every *other* cache by calling that cache's `receiveBusMsg`, which in turn invokes the appropriate `Bus*` snoop method on its coherence protocol. `issueBusMsg` returns whether **any other cache currently holds the line** — this is the wired-OR "shared" signal you need for the E-vs-S decision (see §2.3). In a real architecture a message is simply placed on the bus and every cache sees it; in the simulator each cache is informed separately, but the accounting rule is the same: **one issued message is one bus transaction**, regardless of how many caches snoop it.

Each protocol registers itself with the command line by name using the macro at the top of its `.cc` file:

```cpp
ADD_COHERENCE_TO_CMD_LINE(MSI);      // makes "msi" a valid <protocol> argument (case-insensitive)
```

`dragon.cc` already contains `ADD_COHERENCE_TO_CMD_LINE(Dragon);`, so `dragon` is a valid protocol name once you fill in the logic.

You compile the simulator with a single `make` command; a successful build simply produces the `simulate_cache` executable (there is no banner). Run `make clean` to remove build artifacts.

In this project you maintain coherence across a one-level cache. For simplicity, assume that each processor has a single private L1 cache connected to main memory directly through a shared bus, as in Figure 1.

> **Figure 1.** *A homogeneous SMP system of 16 processors, each with a private L1 cache. All caches are connected to main memory through a shared bus.*

*Note*: The simulator's write policy is **write-back, write-allocate (WBWA)** and it uses the **LRU** replacement policy by default (the engine also ships `fifo` and `rr`). If you write your own simulator, please keep these policies in mind.

### 2.2 Requirements

For this programming assignment, you should implement the **MESI** and **Dragon** protocols and match the results produced by the reference simulator exactly.

Your simulator accepts several arguments that specify attributes of the multiprocessor system, one of which is the coherence protocol. **A single build of `simulate_cache` works with all three protocols (MSI, MESI, Dragon)** — the protocol is chosen by the `<protocol>` command-line argument, not by recompiling. More detail on the arguments is in §3.

You may choose not to use the given simulator and to start from scratch, provided your simulator also uses inheritance to implement the different cache protocols. However, your results **must match the reference simulator exactly**.

### 2.3 Protocol-specific notes

**MESI**: You need a way for the requesting cache to know, at the time of a `BusRd`, whether any other cache holds the line. The standard mechanism is a wired-OR "shared" line on the bus, which the simulator models as the boolean returned by `cache.issueBusMsg(BusRead)`. If it returns `false` (no other cache holds the line), install the block as **E**; otherwise install it as **S**. A later write to an **E** line transitions E→M with **no** bus transaction; a write to an **S** line issues `BusUpgr` (or `BusRdX`).

**Dragon**: On a `PrWr` hit in Sc or Sm, issue a `BusUpd` (`cache.issueBusMsg(BusUpdate)`). The writer transitions to **Sm**; the other sharers stay in Sc (a previous Sm owner is demoted Sm→Sc). Only one cache may be in Sm at a time. On a `PrRd` miss, install the line in **E** if no other cache has the block, and in **Sc** otherwise. On a `PrWr` miss, install the line in **M** if no other cache has the block, and in **Sm** otherwise (broadcasting the initial value via `BusUpd`).

**Writebacks** (`isWriteBackNeeded`): MSI writes back on eviction of **M** lines. MESI writes back on eviction of **M** lines (but *not* **E** — it is clean). Dragon writes back on eviction of **M** *and* **Sm** lines (Sm is dirty), and *not* on eviction of **E** or **Sc**.

---

## 3. Getting Started

### 3.1 Getting the code and traces

Clone the starter code and check out the `program3` branch:

```bash
git clone --branch program3 https://github.com/Mr-Jeffery/CSC-ECE506-summer26.git
cd CSC-ECE506-summer26
```

The **trace files are not included in the repository** (they range from ~750 MB to ~9 GB). Download them from

> <https://drive.google.com/drive/folders/1xjhD4-xCekIL7v5oU1yYe3L5EofdKWcR>

and place them in the `traces/` directory.

Trace files are plain **ASCII text** — one memory reference per line, with three **tab-separated** fields:

```
<processor>	<r|w>	<address>
```

- `processor`: the requesting processor ID, a decimal integer (the provided workloads use 0–15).
- `r|w`: the access type — `r` for a read, `w` for a write.
- `address`: the referenced byte address in hexadecimal (no `0x` prefix).

For example, the line `7	r	0000abcd` means **processor 7 reads address `0xABCD`**. The simulator propagates that request to cache 7, which services it while maintaining coherence.

The **number of caches is inferred from the trace**: a cache is created the first time a processor ID appears (up to 128). The provided workloads use IDs 0–15, i.e. a 16-processor system — there is no separate "number of processors" argument.

On the course infrastructure the traces are also available (from hosts such as `remote.eos.ncsu.edu` that mount ncsudrive) under `/mnt/coe/workspace/csc/CSC506-1/trace/` — copy `streamcluster_simlarge_cleaned` and/or `swaptions_medium_cleaned` into your `traces/` directory. We recommend running on `remote.csc.ncsu.edu`, a virtual server set up for this class; you can transfer files with `scp`/WinSCP.

### 3.2 Building and running

You are given a makefile. If you modify it, do not change the existing targets or their intent; the simulator must build with a single `make`. Building produces the executable `simulate_cache`.

Run the simulator with:

```
./simulate_cache <cache_size> <block_size> <assoc> <protocol> <replacer> <directory> <trace_file> [trace_limit]
```

where—

- `cache_size`: Size of each cache (all caches are the same size). Accepts a `k`/`M` unit suffix, e.g. `1024k` or `2M`. Must be a power of two.
- `block_size`: Block (line) size in bytes. Must be a power of two.
- `assoc`: Associativity (ways per set) of each cache. Must be a power of two; `assoc == cache_size / block_size` gives a fully associative cache.
- `protocol`: Coherence protocol, by name (case-insensitive): `msi`, `mesi`, or `dragon`.
- `replacer`: Replacement policy, by name: `lru` (use this for the report), `fifo`, or `rr`.
- `directory`: Bus/coherence delivery mechanism — use `Broadcast`.
- `trace_file`: Path to the text trace, e.g. `traces/streamcluster_simlarge_cleaned`.
- `trace_limit` *(optional)*: Process only the first *k* records of the trace. Use this to bisect the first point where your output diverges from the reference.

Example:

```
./simulate_cache 1024k 64 4 mesi lru Broadcast traces/streamcluster_simlarge_cleaned
```

The simulator prints one **CSV** row per active processor (see §4 for the columns).

**Batch mode (recommended for the report).** Passing a config *file* instead of the
single-run arguments runs many configurations in a single pass over the trace:

```
./simulate_cache configs/size.txt traces/streamcluster_simlarge_cleaned
```

Each line of a config file is one memory-system configuration
(`<cache_size> <block_size> <assoc> <protocol> <replacer> <directory>`), and every output
row is tagged with a numeric `config` id in its first column. The provided `configs/`
directory already contains the three report sweeps (`size.txt`, `assoc.txt`, `block.txt`)
plus `legend.csv` mapping each `config` id to its parameters. The helper scripts
`q1.sh` (size), `q2.sh` (associativity), `q3.sh` (block size), and `q4.sh` (all three) run
these on both traces and write to `results/q1`–`results/q3`. Expected reference output is
under `results/reference/` (see `results/README.md`).

> **Interactive mode (optional).** Passing a single protocol or replacer name —
> `./simulate_cache <name>` — starts an interactive mode useful for hand-tracing small
> examples. Not required for this assignment.

### 3.3 Debugging against the reference

To help you debug, we provide a reference executable `simulate_cache_ref` that accepts the exact same arguments. Run both and compare:

```
./simulate_cache     1024k 64 4 mesi lru Broadcast traces/streamcluster_simlarge_cleaned > mine.csv
./simulate_cache_ref 1024k 64 4 mesi lru Broadcast traces/streamcluster_simlarge_cleaned > ref.csv
diff mine.csv ref.csv
```

Your output must match the reference **exactly** in both content and format. If it diverges, re-run both with a small `trace_limit` (e.g. `1000`, then `10000`) to pinpoint the first differing record instead of waiting for the whole trace. You can also use the reference to compare protocols against each other (e.g. to confirm that switching MSI→MESI eliminates the `BusUpgr`s you expect). The `test_mesi.sh` and `test_dragon.sh` scripts automate this diff-against-reference workflow.

Ensure that you test all the corner cases and permutations of cache size, associativity, and block size. You are given an executable (`simulate_cache_ref`) rather than fixed validation outputs precisely so that you can stress-test corner cases that fixed validations might miss.

---

## 4. Report

For this problem, you will experiment with various cache configurations and measure the cache performance of the system. The cache configurations that you should try are:

- **Cache size:** vary over 128KB, 256KB, 512KB, 1024KB, 2048KB while keeping associativity at 4 and block size at 64B.
- **Cache associativity:** vary over 1, 2, 4, 8, fully associative while keeping cache size at 1024KB and block size at 64B.
- **Cache-block size:** vary over 32B, 64B, 128B, 256B, 512B while keeping cache size at 2048KB and associativity at 2.
- **Protocol:** MSI, MESI, and Dragon.

Keep the replacement policy at `lru` throughout. Do all the above experiments for each protocol. (Note: the fully-associative configuration searches every line on each access, so it is far slower than the others. Run it on **streamcluster only**; for the larger `swaptions` trace, cap the associativity sweep at 8-way — `q2.sh` already does this.)

### 4.1 Statistics collected

For each run the simulator emits one CSV row per active processor. The columns are:

```
config, core, miss rate,
processor reads, processor writes,
bus reads, bus readxs, bus updates, bus upgrades, bus writes,
read misses, write misses,
line flushes, line fetches, c2c transfers, write backs, memory writes,
evictions, exclusions, interventions, invalidations,
useful bus updates
```

`config` is `0` for a single command-line run; `core` is the processor ID. Map these columns onto the statistics your report must present:

| Report statistic | CSV column(s) |
| :-- | :-- |
| Read transactions received | `processor reads` |
| Read misses | `read misses` |
| Write transactions received | `processor writes` |
| Write misses | `write misses` |
| Total miss rate | `miss rate` |
| Dirty blocks written back | `write backs` |
| Transactions with memory (reads + fetches + writebacks) | `line fetches` + `memory writes` + `write backs` |
| Cache-to-cache transfers (received by this cache) | `c2c transfers` |
| Interventions | `interventions` |
| Invalidations | `invalidations` |
| Flushes | `line flushes` |
| BusRds issued | `bus reads` |
| BusRdXs issued (MSI, MESI) | `bus readxs` |
| BusUpgrs issued (MSI, MESI) | `bus upgrades` |
| BusUpds issued (Dragon) | `bus updates` |
| Useful BusUpds (Dragon) | `useful bus updates` |

A **useful BusUpd** is an update the simulator delivered to a cache whose line was then **read by the local processor before the line was evicted or locally overwritten**; an update whose line is evicted (or overwritten by a local write) before any such read is "wasted." This counter is maintained by the engine, so it is populated automatically once your Dragon implementation issues and handles `BusUpd` correctly.

For the bus operations, count the number *issued* on the bus. Do not count one bus transaction for each cache the operation is applied to — if P1 issues a `BusRd` in a 16-processor system, that is **one** bus operation, not sixteen.

### 4.2 Discussion

Overall, the report should—

- Present the statistics in **tabular** format as well as **figures**.
- Discuss trends with respect to changes in system configuration as well as across protocols.
- Consider the following questions:
  - Which helps more on miss rate — doubling associativity, doubling block size, or doubling cache size? Does the answer depend on the protocol?
  - Does higher associativity benefit a large cache or a small cache more?
  - Compare MSI and MESI head-to-head. How much does the Exclusive state actually reduce bus traffic? On which configurations and which traces does it help most? Quantify the reduction in `bus readxs` + `bus upgrades`.
  - Compare invalidation-based (MESI) vs update-based (Dragon) coherence. Which protocol issues fewer bus transactions in total? Which causes fewer cache-to-cache transfers? Which causes fewer memory transactions? Are the answers consistent across traces?
  - What fraction of Dragon's `bus updates` are "useful" (`useful bus updates` / `bus updates`) vs "wasted"? Use this to argue when update-based coherence is appropriate.
  - How does Dragon's bus traffic scale with block size compared to MESI? Why?
  - Identify a sharing pattern in your traces (e.g. migratory data, producer-consumer, or read-mostly) and explain which protocol you would expect to handle it best. Do your numbers confirm this?

---

## 5. Grading

- **20%**: Your code compiles successfully and produces output for all three protocols.
- **30%**: Correctness of results (exact match with the reference simulator).
- **50%**: Report. Credit is based on the statistics shown and the quality of the discussion.

---

## 6. Submission

Create a compressed folder named `<unityID1[_unityID2]>.tar.gz` containing—

- The code directory including all `.cc` files. (Do not include object files; run `make clean` before submission.)
- A report of your results, including all the statistics mentioned in Section 4, named `report.pdf`.

Any deviation from the format mentioned for the files and zip folder will result in a deduction of 5 points.

The command to compress:

```
tar -czvf <unityID1[_unityID2]>.tar.gz /path/to/your/program3
```

---

## 7. Suggestions

- Read the `Cache` class and the complete `MSI` protocol carefully, and understand how a single cache works before touching any protocol code.
- Most of the code is well encapsulated, so you should only need to edit `src/coherence/mesi.cc` and `src/coherence/dragon.cc` (and their headers). You should not need to modify the `Cache` engine.
- In `src/typedefs.h` the `state_e` enum already defines `E`, `M`, `Sc`, and `Sm`, so you do not need to add new states — MESI uses `E`; Dragon uses `E`, `M`, `Sc`, and `Sm`.
- For MESI, implement the wired-OR shared signal using the boolean returned by `cache.issueBusMsg(BusRead)`: `false` ⇒ install E, `true` ⇒ install S.
- For Dragon, be careful with the Sm→Sc transition when ownership transfers. At any time at most one cache should be in Sm for a given block. The cleanest invariant: a `BusUpd` issuer becomes Sm, and any other cache that was previously Sm transitions to Sc.
- Implement MESI first — it is a small delta from MSI and lets you validate your testing workflow before you tackle Dragon, which is structurally more different.
- Build incrementally and use `trace_limit` with small values (e.g. 1000, 10000) plus `diff` against `simulate_cache_ref`. Don't wait until the full trace finishes to discover a bug.
- Start early and post your questions on Ed.
