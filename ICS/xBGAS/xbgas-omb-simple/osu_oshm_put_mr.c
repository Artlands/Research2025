#define BENCHMARK "OSU OpenSHMEM Put Message Rate (MB/s) Test"

/*
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level directory.
 */

 /* WITH FENCE AFTER PUT
# OSU OpenSHMEM Put Message Rate (MB/s) Test - 100ns
# Size                    MB/s
---------1                   0.990
---------2                   2.012
---------4                   4.104
---------8                   8.345
--------16                  16.564
--------32                  32.151
--------64                  66.256
-------128                 132.484
-------256                 264.967
-------512                 529.589
------1024                1058.719
------2048                2114.229
------4096                3069.021
------8192                4587.493
-----16384                5364.254
-----32768                6300.403
-----65536                6656.584

# OSU OpenSHMEM Put Message Rate (MB/s) Test - 200ns
# Size                    MB/s
---------1                   0.684
---------2                   1.375
---------4                   2.785
---------8                   5.691
--------16                  11.210
--------32                  21.965
--------64                  44.839
-------128                  89.678
-------256                 179.331
-------512                 358.608
------1024                 716.796
------2048                1432.120
------4096                2280.356
------8192                3644.910
-----16384                4657.228
-----32768                5787.037
-----65536                6358.087
*/

#include <xbrtime.h>
#include "xbgas_osu_util.h"


int loop = 10;

struct pe_vars {
    int me;
    int npes;
    int pairs;
    int nxtpe;
};

struct pe_vars init_xbrtime(void)
{
    struct pe_vars v;

    xbrtime_init();
    v.me = xbrtime_mype();
    v.npes = xbrtime_num_pes();

    v.pairs = v.npes / 2;
    v.nxtpe = v.me < v.pairs ? v.me + v.pairs : v.me - v.pairs;

    return v;
}

void check_usage(int me, int npes, int argc, char *argv[])
{
    if (2 > npes) {
        if (0 == me) {
            printf("This test requires at least two processes\n");
        }

        exit(EXIT_FAILURE);
    }
}

void print_header_local(int myid)
{
    if (myid == 0) {
        printf(HEADER);
        printf("%-*s%*s\n", 10, "# Size", FIELD_WIDTH, "MB/s");
    }
}

char *allocate_memory(int me, long align_size)
{
    char *msg_buffer;

    msg_buffer = (char *)xbrtime_align(align_size, MAX_MESSAGE_SIZE * loop);

    if (NULL == msg_buffer) {
        printf("Failed to xbrtime_align (pe: %d)\n", me);
        exit(EXIT_FAILURE);
    }

    return msg_buffer;
}

double message_rate(struct pe_vars v, char *buffer, unsigned long size,
                    int iterations)
{
    double begin, end;
    double total = 0.0;
    double rate;
    int i, offset;

    memset(buffer, size, MAX_MESSAGE_SIZE * loop);

    xbrtime_barrier_all();

    if (v.me < v.pairs) {
        begin = TIME();

        for (i = 0, offset = 0; i < iterations; i++, offset += size) {
            xbrtime_putmem(&buffer[offset], &buffer[offset], size, v.nxtpe);
        }
        xbrtime_quiet();
        end = TIME();
        
        total = ((double)end - (double)begin ); // us
        rate = ( (iterations * size)/(1024.0 * 1024.0) * 1e6) / total; // MB/s
        return rate;
    }

    return 0;
}

void print_message_rate(int myid, unsigned long size, double rate)
{
    if (myid == 0) {
        printf("%-*d%*.*f\n", 10, size, FIELD_WIDTH, FLOAT_PRECISION, rate);
    }
}

void benchmark(struct pe_vars v, char *msg_buffer)
{
    static double mr, mr_sum;
    unsigned long size, i;

    /*
     * Warmup
     */
    if (v.me < v.pairs) {
        for (i = 0; i < (MAX_MESSAGE_SIZE * loop); i += MAX_MESSAGE_SIZE) {
            xbrtime_putmem(&msg_buffer[i], &msg_buffer[i], MAX_MESSAGE_SIZE, v.nxtpe);
        }
    }

    xbrtime_barrier_all();

    /*
     * Benchmark
     */
    for (size = 1; size <= MAX_MESSAGE_SIZE; size <<= 1) {
        mr = message_rate(v, msg_buffer, size, loop);
        xbrtime_double_sum_reduce(&mr_sum, &mr, 1);
        print_message_rate(v.me, size, mr_sum);
    }
}

int main(int argc, char *argv[])
{
    struct pe_vars v;
    char *msg_buffer, *aligned_buffer;
    long alignment;

    /*
     * Initialize
     */
    v = init_xbrtime();
    check_usage(v.me, v.npes, argc, argv);
    print_header_local(v.me);

    /*
     * Allocate Memory
     */
    alignment = 8;
    msg_buffer = allocate_memory(v.me, alignment);
    memset(msg_buffer, 0, MAX_MESSAGE_SIZE * loop);

    /*
     * Time Put Message Rate
     */
    benchmark(v, msg_buffer);

    /*
     * Finalize
     */
    xbrtime_free(msg_buffer);
    xbrtime_close();

    return EXIT_SUCCESS;
}
