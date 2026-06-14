#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {
    int a = 20, i, id;

    /* FIXME: Add private, firstprivate, or lastprivate clause for 'a'
     * to the directive below. Run with each clause and observe the
     * output inside and outside the parallel region.
     *   private(a)      -- a is uninitialized inside each thread; a=20 unchanged outside
     *   firstprivate(a) -- a starts as 20 inside each thread; a=20 unchanged outside
     *   lastprivate(a)  -- a is private inside; last iteration (i=2) copies back a=15 outside
     */
    #pragma omp parallel for
    for (i = 0; i < 3; i++){
        id = omp_get_thread_num();
        printf("Value inside from Thread %d, i=%d: a = %d\n", id, i, a);
        a = (i + 1) * 5;
    }

    printf("Value outside parallel region, a = %d\n", a);

    return 0;
}
