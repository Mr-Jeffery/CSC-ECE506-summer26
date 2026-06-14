#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {

    //#pragma omp parallel
    printf("part1\n");

    //#pragma omp parallel
    printf("part2\n");

    //#pragma omp parallel
    printf("part3\n");

    return 0;
}

