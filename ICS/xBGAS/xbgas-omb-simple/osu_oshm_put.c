#define BENCHMARK "OSU OpenSHMEM Put Test"

/*
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level directory.
 */

/* WITH FENCE AFTER PUT
# OSU OpenSHMEM Put Test - 100ns
# Size           Latency (us)
---------1                   1.309
---------2                   1.303
---------4                   1.303
---------8                   1.303
--------16                   1.303
--------32                   1.303
--------64                   1.303
-------128                   1.303
-------256                   1.303
-------512                   1.304
------1024                   1.304
------2048                   1.305
------4096                   1.307
------8192                   1.371
-----16384                   1.580
-----32768                   1.857
-----65536                   2.196

# OSU OpenSHMEM Put Test - 200ns
# Size           Latency (us)
---------1                   2.109
---------2                   2.104
---------4                   2.103
---------8                   2.104
--------16                   2.103
--------32                   2.104
--------64                   2.104
-------128                   2.104
-------256                   2.104
-------512                   2.104
------1024                   2.104
------2048                   2.105
------4096                   2.107
------8192                   2.171
-----16384                   2.382
-----32768                   2.657
-----65536                   2.997
*/

/* WITHOUT FENCE AFTER PUT
# OSU OpenSHMEM Put Test - 100ns
# Size           Latency (us)
---------1                   0.901
---------2                   0.895
---------4                   0.894
---------8                   0.894
--------16                   0.895
--------32                   0.895
--------64                   0.895
-------128                   0.894
-------256                   0.894
-------512                   0.895
------1024                   0.895
------2048                   0.894
------4096                   0.899
------8192                   0.940
-----16384                   1.167
-----32768                   1.438
-----65536                   1.880

# OSU OpenSHMEM Put Test - 200ns
# Size           Latency (us)
---------1                   1.300
---------2                   1.295
---------4                   1.295
---------8                   1.294
--------16                   1.294
--------32                   1.295
--------64                   1.295
-------128                   1.294
-------256                   1.295
-------512                   1.294
------1024                   1.295
------2048                   1.294
------4096                   1.298
------8192                   1.341
-----16384                   1.567
-----32768                   1.839
-----65536                   2.281
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
                xbrtime_putmem(r_buf, s_buf, size, 1);
                // xbrtime_quiet();
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
