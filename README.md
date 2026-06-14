# CSC/ECE 506: Architecture of Parallel Computers
## Program 2: OpenMP Programming

**Due:** Friday, June 19, 2026

This set of six short programming exercises will familiarize you with OpenMP and the shared-memory programming model.

---

## Procedure

1. Clone the repository and check out the `program2` branch:
   ```bash
   git clone --branch program2 https://github.com/Mr-Jeffery/CSC-ECE506-summer26.git
   cd CSC-ECE506-summer26
   ```
2. `cd` into each subdirectory (`p1` through `p6`) in succession.
3. Check that you can compile each example by executing `make` in each subdirectory.

---

## Deliverables

Compress the `program2` directory into a `.tgz` file using the following command:

```bash
tar -cvzf <unityID>.tgz program2/
```

and submit it to Moodle.

Your `.tgz` file should contain:

1. The files specified in square brackets next to each question, with their requested contents.
2. A `report.pdf` file that includes all the explanations that you are asked to provide below.

> **Note:** To avoid breaking the grading scripts and causing extra hassle for the TA, please follow these naming conventions strictly.

---

## Problems

### Problem 1 *(5 points each)*

**(a)** Uncomment the pragma in line 9 of `p1.c`, recompile it, and check its output. How many `"part2"`s do you get with or without the pragma? Why?

**(b)** Modify the pragma in line 9 of `p1.c` to output three `"part2"`s, and write the modified file to `p1b.c`.

**(c)** Uncomment line 6 and line 12 of `p1.c` and add one OMP directive of the form `omp_set_num_threads()` in your solution of part (b) to output two `"part1"`s, three `"part2"`s, and two `"part3"`s. Put your code into `p1c.c`. Explain your solution.

---

### Problem 2 *(10 points)*

**(a)** Modify line 7 and fix line 9 and line 10 of `p2.c` to generate the following output, except that it is OK if your thread IDs print in a different order: [`p2a.c`]

```
Hello from thread 0 of 10
Hello from thread 1 of 10
Hello from thread 2 of 10
Hello from thread 3 of 10
Hello from thread 9 of 10
Hello from thread 4 of 10
Hello from thread 5 of 10
Hello from thread 6 of 10
Hello from thread 7 of 10
Hello from thread 8 of 10
```

**(b)** *(5 points)* Modify the above program to use the default number of threads. [`p2b.c`]

---

### Problem 3 *(10 points)*

Fix line 13 of `p3.c` to set all entries in the array to `"1"` with the `private` clause. Explain why the change is needed. [`p3.c`]

---

### Problem 4 *(5 points each)*

**(a)** Explain why `p4.c` does not calculate the correct "parallel sum" and fix it using the `reduction` clause. [`p4a.c`]

**(b)** Fix `p4.c` using the `atomic` clause (instead of fixing it the way you did in part (a)). [`p4b.c`]

**(c)** Now fix `p4.c` by using the `critical` clause (instead of the fixes of parts (a) and (b)). [`p4c.c`]

**(d)** Explain the difference between part (b) and part (c).

---

### Problem 5 *(10 points)*

Fix the `#pragma` directive in `p5.c` to use `private`, `lastprivate`, and `firstprivate` clauses one at a time. For each clause, run the program and record the output in your `report.pdf`. Explain why the value of `a` outside the parallel region differs across the three cases. [`p5.c`]

> **Note:** Submit one `p5.c` file. The comment block in the file explains all three clauses. Only the clause keyword on the `#pragma` line changes between runs.

---

### Problem 6

**(a)** *(20 points)* Calculate π using numerical integration (the midpoint rule):

$$\int_0^1 \frac{4}{1+x^2}\,dx = \pi \approx h\sum_{i=0}^{N-1}\frac{4}{1+\left((i+0.5)\cdot h\right)^2}, \quad h = \frac{1}{N},\quad N = \texttt{0x10000000}$$

Start from `p6.c`. Use `omp parallel for` with a `reduction` clause to parallelize the summation. Insert `omp_get_wtime()` calls to time the parallel region and compare against the serial version. [`p6a.c`]

**(b)** *(10 points)* Repeat part (a) using `schedule(static)` and `schedule(dynamic, <chunk>)`. Record the timing for each and explain why one performs better for this computation. [`p6b.c`]