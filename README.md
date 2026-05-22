#### ECE/CSC 506: Architecture of Parallel Computers
# Program 1: GPU Programming
**Due: May 29, 2026 11:59pm**

## Introduction

GPUs outperform CPUs on applications like image processing, array processing, and plotting graphs. This advantage derives from GPUs' ability to process data in parallel rather than serially.

This assignment asks you to code vector calculations for a graph plot using CUDA code. This assignment has three objectives:

* To introduce you to CUDA.
* To introduce you to simulation environments.
* To demonstrate how small modifications in the code can reduce the number of instructions processed by the processor, the number of cache accesses, and overall energy usage.



## Background

![][image1]

GPUs generally require a host device to activate them. The CPU (host) will run most of the serial code, but for the parallelizable code, it invokes the GPU kernel.

Note these attributes of CUDA code:
* The kernel invocation is identified by:
  ```cpp
  <<< blocksPerGrid, threadsPerBlock >>>
  ```
* The GPU kernel function header is:
  ```cpp
  __global__ void <function name> (args....)
  ```
* `cudaMemcpy` is used to copy the data in between device and host.

Watch the following videos from the course lecture schedule to learn more about programming the GPU. You may take the embedded quizzes, but you are not required to. They will not be counted toward your grade.

a. Introduction to CUDA, data parallelism and threads [23:15] [**Watch**](https://moodle-courses2527.wolfware.ncsu.edu/mod/lti/view.php?id=1193758)  
b. Introduction to heterogeneous parallel computing [16:54] [**Watch**](https://moodle-courses2527.wolfware.ncsu.edu/mod/lti/view.php?id=1193756)  
c. Portability and scalability in heterogeneous parallel computing [9:12] [**Watch**](https://moodle-courses2527.wolfware.ncsu.edu/mod/lti/view.php?id=1193757)  
d. Multiplying matrices [5:00] [**Watch**](https://moodle-courses2527.wolfware.ncsu.edu/mod/lti/view.php?id=1193786)

**Additional Resources**: [CUDA Programming Guide](https://docs.nvidia.com/cuda/cuda-c-programming-guide/)


## Experiment
### Environment setup

To see the impact of the four parameters mentioned above on your code, we use gpgpusim to simulate the working of GPU. This simulator simulates the working of the **NVIDIA Tesla V100**.  
To set up the environment on your PC, follow the instructions in [`Docker_setup_506.md`](Docker_setup_506.md)

> **NOTE**: Do not update any installed packages in the Linux environment. The simulator requires a certain version of packages (e.g., `gcc`) to function smoothly. Kindly leave the packages untouched.

### Compile and Run a CUDA Program in the Simulator

**1. Download the vectorAdd example from GitHub**
Run this in your host folder:
```bash
git clone -b program1 https://github.com/Mr-Jeffery/CSC-ECE506-summer26.git
```

**2. Navigate to the directory and compile the program**
Run the following commands inside container. Ensure the NVCC flag `--cudart=shared` is used (notice this compiler flag is already configured in the provided `Makefile`).

```bash
cd ~/vectorAdd/
make clean
make
```

**3. Copy the simulator configuration files to your directory**

```bash
cp ~/gpgpu-sim_distribution/configs/tested-cfgs/SM7_QV100/* .
```

> **Note:** Pause and take a look at the `gpgpusim.config` file that you just copied. This file dictates the simulator parameters, such as the number of cores, cache sizes, and other hardware specifics.

**4. Run the program to get the simulation output**

```bash
./vectorAdd > output.txt 
```

### Programming Assignment: Mandelbrot Set

For this assignment, you will write a CUDA program to compute the Mandelbrot set, which will then be visualized using a Python script. Skeleton code is provided in the `mandelbrot/` directory.

**Task 1: Device Kernel (`mandelbrotKernel`)**
Complete the kernel in `mandelbrot.cu` to calculate the Mandelbrot set:
1. Determine the thread's pixel coordinate `(x, y)` using `blockIdx`, `blockDim`, and `threadIdx`.
2. Ensure the thread is within the boundary of the image (`width` and `height`).
3. Map the `(x, y)` coordinate to the complex plane component `c`. Adjust the scale properly to map the pixel coordinates.
4. Loop iteratively using the formula $z = z^2 + c$ until the magnitude of $z$ escapes 2 or the iteration hits `max_iter`.
5. Store the iteration count into the flattened 1D array (`d_out`).

**Task 2: Host Code (`main`)**
Complete the main host configuration in `mandelbrot.cu`:
1. Use `cudaMalloc` to allocate GPU memory for the output.
2. Define the grid and block dimensions (e.g., `dim3 threadsPerBlock(16, 16);` and the corresponding grid size based on `width` and `height`).
3. Call the `mandelbrotKernel`.
4. Use `cudaMemcpy` to pull the results from the device into the `h_out` CPU array.
5. Use `cudaFree` to clean up memory.

**Task 3: Visualization**
After successful execution, your CUDA program will produce a raw binary file `mandelbrot.bin`. You can then run the `visualize.py` script provided in the `mandelbrot` directory.
The script will read the raw binary file and create `mandelbrot.png`, visualizing the Mandelbrot set.

> **Note:** You do not have to run the Mandelbrot program on the simulator, but do include your setting if you are using your own machine.

## Submission format

Your submission should contain the `mandelbrot` folder (with your completed code and generated outputs) and a report file in a zip archive. The whole folder structure should look like the following:

```text
* mandelbrot
  * mandelbrot.cu
  * mandelbrot.png
  * Makefile
* report.pdf
```
Your report should at least contain these parts:
- what is the Instruction Per Cycle(IPC) of your `vectorAdd` program on the simulator and how is it calculated
- briefly explain your Mandelbrot program implementation
- how to run your Mandelbrot program

The `mandelbrot` folder should contain your fractal image `mandelbrot.png`, `mandelbrot.cu` source file and a `Makefile`. Your program should be able to compile when `make` is issued in that folder.

[image1]: <data:image/png;base64,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>
