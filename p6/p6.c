#include <stdio.h>

#define N 0x10000000

int main(int argc, char** argv) {
    int i;
    double h, x, pi;

    h  = 1.0 / N;
    pi = 0.0;

    for (i = 0; i < N; i++) {
        x   = (i + 0.5) * h;
        pi += 4.0 / (1.0 + x * x);
    }
    pi *= h;

    printf("pi = %.10f\n", pi);

    return 0;
}
