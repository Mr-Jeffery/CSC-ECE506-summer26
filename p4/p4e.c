#include <stdio.h>
#include <omp.h>

#define THREADS 3
#define REPS    3

int main(int argc, char** argv) {
    int sum = 0;

    #pragma omp parallel num_threads(THREADS)
    {
        int i, t;
        for (i = 0; i < REPS; i++) {
            t   = sum;  /* load  */
            t  += 1;    /* add   */
            sum = t;    /* store */
        }
    }

    printf("sum = %d  (max possible: %d)\n", sum, THREADS * REPS);
    return 0;
}
