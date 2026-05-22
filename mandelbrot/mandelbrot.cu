#include <iostream>
#include <fstream>
#include <vector>

// Macro to catch and print CUDA errors
#define checkCuda(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line) {
    if (code != cudaSuccess) {
        std::cerr << "CUDA Error: " << cudaGetErrorString(code) 
                  << " " << file << " " << line << std::endl;
        exit(code);
    }
}

// ---------------------------------------------------------
// DEVICE CODE (Runs on the GPU)
// The __global__ keyword tells the compiler this is a Kernel function.
// It is called by the CPU but executes on the GPU.
// ---------------------------------------------------------
__global__ void mandelbrotKernel(int* d_out, int width, int height, int max_iter) {
    // 1. Find out which pixel this specific GPU thread is supposed to calculate.
    // blockIdx and threadIdx are built-in variables that give the thread its unique ID.

    // Boundary check: If the thread is outside our image dimensions, do nothing.

    // 2. Map the pixel coordinates (x, y) to the complex plane (c_re, c_im).
    // This scales our image to view the area from roughly -2.0 to 1.0 on the X axis.

    // 3. Apply the Mandelbrot formula: z = z^2 + c

    // We loop until the magnitude of z is greater than 2 (escapes) or we hit max iterations.

    // 4. Save the number of iterations it took to escape to our output array.
    // We flatten the 2D image into a 1D array using (y * width + x).
}


// ---------------------------------------------------------
// HOST CODE (Runs on the CPU)
// ---------------------------------------------------------
int main() {
    // Image settings
    int width = 2048;
    int height = 2048;
    int max_iter = 1000;
    size_t num_pixels = width * height;
    size_t size_in_bytes = num_pixels * sizeof(int);

    // 1. Allocate memory on the CPU (Host)
    std::vector<int> h_out(num_pixels);

    // 2. Allocate memory on the GPU (Device), remember to wrap CUDA calls in our checkCuda macro
    // TODO

    // 3. Set up the CUDA Grid and Blocks.
    // GPUs group threads into "Blocks". We use a 16x16 block of threads.
    // The "Grid" is how many blocks we need to cover the whole image.
    // TODO

    std::cout << "Calculating Mandelbrot set on GPU..." << std::endl;

    // 4. Launch the GPU Kernel
    // TODO

    // Check if the kernel failed to launch (e.g., due to architecture mismatch)
    checkCuda(cudaGetLastError());
    
    // Wait for GPU to finish and check for execution errors
    checkCuda(cudaDeviceSynchronize());

    // 5. Copy the results back from the GPU to the CPU
    // TODO

    // 6. Save the data to a raw binary file for Python to read
    std::cout << "Saving data to mandelbrot.bin..." << std::endl;
    std::ofstream outfile("mandelbrot.bin", std::ios::binary);
    outfile.write(reinterpret_cast<char*>(h_out.data()), size_in_bytes);
    outfile.close();

    // 7. Clean up GPU memory
    // TODO

    std::cout << "Done!" << std::endl;
    return 0;
}