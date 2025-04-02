#define BENCHMARK "OSU OpenSHMEM Put Message Rate (MB/s) Test"

/*
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level directory.
 */

/*
# OSU OpenSHMEM Put Message Rate (MB/s) Test
# Size                    MB/s
---------1                   0.990
---------2                   2.011
---------4                   4.101
---------8                   8.316
--------16                  16.436
--------32                  31.605
--------64                  62.755
-------128                  90.476
-------256                 113.269
-------512                 129.662
------1024                 139.733
------2048                 145.385
------4096                 148.389
------8192                 149.944
-----16384                 150.697
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
