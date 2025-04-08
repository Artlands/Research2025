#define BENCHMARK "OSU OpenSHMEM Get Test"

/*
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level directory.
 */

/*
# OSU OpenSHMEM Get Test - 200ns
# Size           Latency (us)
---------1                   0.805
---------2                   0.804
---------4                   0.804
---------8                   0.804
--------16                   0.804
--------32                   0.804
--------64                   0.804
-------128                   0.804
-------256                   0.804
-------512                   0.804
------1024                   0.804
------2048                   0.804
------4096                   0.844
------8192                   0.868
-----16384                   1.046
-----32768                   1.307
-----65536                   1.966
----131072                   5.316
----262144                  38.208

# OSU OpenSHMEM Get Test - 200ns
# Size           Latency (us)
---------1                   1.212
---------2                   1.206
---------4                   1.206
---------8                   1.206
--------16                   1.206
--------32                   1.206
--------64                   1.206
-------128                   1.206
-------256                   1.206
-------512                   1.206
------1024                   1.206
------2048                   1.206
------4096                   1.246
------8192                   1.270
-----16384                   1.451
-----32768                   1.706
-----65536                   2.364
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
                xbrtime_getmem(r_buf, s_buf, size, 1);
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

