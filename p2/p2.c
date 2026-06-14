#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {
    printf("\n");

    #pragma omp parallel /* FIXME */
    {
        int id = 0; /* FIXME: Insert the id of the current thread */
        int numThreads = 0; /* FIXME: Insert total number of threads */
        printf("Hello from thread %d of %d\n", id, numThreads);
    }

    printf("\n");
    return 0;
}

