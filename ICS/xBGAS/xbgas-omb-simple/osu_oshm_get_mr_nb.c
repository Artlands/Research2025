#define BENCHMARK "OSU OpenSHMEM Get_nb Message Rate (MB/s) Test"

/*
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level directory.
 */

 /*
# OSU OpenSHMEM Get_nb Message Rate (MB/s)Test (1 iteration)
# Size                    MB/s
---------1                   0.391
---------2                   1.255
---------4                   2.921
---------8                   5.771
--------16                  11.025
--------32                  22.050
--------64                  44.486
-------128                  87.193
-------256                 167.220
-------512                 302.904
------1024                 518.896
------2048                 792.021
------4096                1066.699
------8192                1300.350
-----16384                1460.553

# OSU OpenSHMEM Get_nb Message Rate (MB/s) Test (10 iteration)
# Size                    MB/s
---------1                   0.924
---------2                   2.010
---------4                   4.097
---------8                   8.449
--------16                  16.503
--------32                  31.969
--------64                  63.724
-------128                 108.546
-------256                 142.489
-------512                 168.315
------1024                 185.109
------2048                 194.837
------4096                 200.099
------8192                 202.780
-----16384                 204.004
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
            xbrtime_getmem_nbi(&buffer[offset], &buffer[offset], size, v.nxtpe);
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
