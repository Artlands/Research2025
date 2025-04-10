#define BENCHMARK "OSU OpenSHMEM Get_nb Test"

/*
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level directory.
 */

/*
# OSU OpenSHMEM Get_nb Test
# Size           Latency (us)
---------1                   1.225
---------2                   1.219
---------4                   1.218
---------8                   1.218
--------16                   1.219
--------32                   1.219
--------64                   1.218
-------128                   1.219
-------256                   1.218
-------512                   1.218
------1024                   1.219
------2048                   1.219
------4096                   1.260
------8192                   1.296
-----16384                   1.485
-----32768                   2.039
-----65536                   2.707
*/

#include <xbrtime.h>
#include "xbgas_osu_util.h"

int loop = 10;
int warmup = 5;


int main(int argc, char *argv[])
{
    int myid, numprocs, i;
    int size;
    char *s_buf, *r_buf;
    double t_start = 0, t_end = 0;

    xbrtime_init();
    myid = xbrtime_mype();
    numprocs = xbrtime_num_pes();

    if (numprocs != 2) {
        if (myid == 0) {
            printf("This test requires exactly two processes\n");
        }

        return EXIT_FAILURE;
    }

    /**************Allocating Memory*********************/

    s_buf = (char *)xbrtime_align(MESSAGE_ALIGNMENT, MYBUFSIZE);
    r_buf = (char *)xbrtime_align(MESSAGE_ALIGNMENT, MYBUFSIZE);

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

        xbrtime_barrier_all();

        if (myid == 0) {
            for (i = 0; i < warmup + loop; i++) {
                if ( i == warmup) {
                    t_start = TIME();
                }
                xbrtime_getmem_nbi(r_buf, s_buf, size, 1);
                xbrtime_quiet();
            }

            t_end = TIME();
        }
        xbrtime_barrier_all();

        if (myid == 0) {
            double latency = (1.0 * (t_end - t_start)) / loop;
            printf("%-*d%*.*f\n", 10, size, FIELD_WIDTH, FLOAT_PRECISION, latency);
        }
    }

    xbrtime_barrier_all();

    xbrtime_free(s_buf);
    xbrtime_free(r_buf);

    xbrtime_close();

    return EXIT_SUCCESS;
}
