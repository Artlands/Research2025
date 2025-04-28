#define BENCHMARK "OSU OpenSHMEM Put Test"

/*
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level directory.
 */

/*
*/


#include <shmem.h>
#include "osu_util.h"

int skip = OSHM_SKIP_SMALL;
int loop = OSHM_SKIP_SMALL;

int main(int argc, char *argv[])
{
    int myid, numprocs, i;
    int size;
    char *s_buf, *r_buf;
    char *s_buf_heap, *r_buf_heap;
    int align_size;
    double t_start = 0, t_end = 0;

    shmem_init();
    myid = shmem_my_pe();
    numprocs = shmem_n_pes();
    
    if (numprocs != 2) {
        if (myid == 0) {
            printf("This test requires exactly two processes\n");
        }
        return EXIT_FAILURE;
    }

    align_size = MESSAGE_ALIGNMENT;

    /**************Allocating Memory*********************/

    s_buf_heap = (char *)shmalloc(MYBUFSIZE);
    r_buf_heap = (char *)shmalloc(MYBUFSIZE);

    s_buf = (char *)(((unsigned long)s_buf_heap + (align_size - 1)) /
                         align_size * align_size);

    r_buf = (char *)(((unsigned long)r_buf_heap + (align_size - 1)) /
                        align_size * align_size);

    /**************Memory Allocation Done*********************/

    if (myid == 0) {
        printf(HEADER);
        printf("%-*s%*s", 10, "# Size", FIELD_WIDTH, "Latency (us)\n");
    }

    for (size = 1; size <= MAX_MSG_SIZE_PT2PT; size <<= 1) {
        /* touch the data */
        for (i = 0; i < size; i++) {
            s_buf[i] = 'a';
            r_buf[i] = 'b';
        }

        if ( size > LARGE_MESSAGE_SIZE ) {
            loop = OSHM_LOOP_LARGE;
            skip = OSHM_SKIP_LARGE;
        }

        shmem_barrier_all();

        if (myid == 0) {
            for (i = 0; i < loop + skip; i++) {
                if ( i == skip) {
                    t_start = TIME();
                }
                shmem_putmem(r_buf, s_buf, size, 1);
            }

            t_end = TIME();
        }
        shmem_barrier_all();

        if (myid == 0) {
            double latency = (1.0 * (t_end - t_start)) / loop;
            printf("%-*d%*.*f\n", 10, size, FIELD_WIDTH, FLOAT_PRECISION, latency);
        }
    }

    shmem_barrier_all();

    shmem_free(s_buf);
    shmem_free(r_buf);

    shmem_finalize();

    return EXIT_SUCCESS;
}

