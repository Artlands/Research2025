#define BENCHMARK "OSU OpenSHMEM Get_nb Message Rate (MB/s) Test"

/*
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level directory.
 */

/*
*/

#include <shmem.h>
#include "osu_util.h"


int loop = 1000;

struct pe_vars {
    int me;
    int npes;
    int pairs;
    int nxtpe;
};

struct pe_vars init_shmem(void)
{
    struct pe_vars v;

    shmem_init();
    v.me = shmem_my_pe();
    v.npes = shmem_n_pes();

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

    msg_buffer = (char *)shmem_malloc(MAX_MESSAGE_SIZE * loop + align_size);

    if (NULL == msg_buffer) {
        printf("Failed to shmem_align (pe: %d)\n", me);
        exit(EXIT_FAILURE);
    }

    return msg_buffer;
}

char *align_memory(unsigned long address, int const align_size)
{
    return (char *)((address + (align_size - 1)) / align_size * align_size);
}

double message_rate(struct pe_vars v, char *buffer, unsigned long size,
                    int iterations)
{
    double begin, end;
    double total = 0.0;
    double rate;
    int i, offset;

    memset(buffer, size, MAX_MESSAGE_SIZE * loop);

    shmem_barrier_all();

    if (v.me < v.pairs) {
        begin = TIME();

        for (i = 0, offset = 0; i < iterations; i++, offset += size) {
            shmem_getmem_nbi(&buffer[offset], &buffer[offset], size, v.nxtpe);
        }

        shmem_quiet();
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
        printf("%-*ld%*.*f\n", 10, size, FIELD_WIDTH, FLOAT_PRECISION, rate);
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
            shmem_putmem(&msg_buffer[i], &msg_buffer[i], MAX_MESSAGE_SIZE, v.nxtpe);
        }
    }

    shmem_barrier_all();

    /*
     * Benchmark
     */
    for (size = 1; size <= MAX_MESSAGE_SIZE; size <<= 1) {
        mr = message_rate(v, msg_buffer, size, loop);
        // Manual reduction using shmem_double_sum_to_all
        double pWork[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
        long int pSync[SHMEM_REDUCE_SYNC_SIZE];
        for (i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i++) {
            pSync[i] = SHMEM_SYNC_VALUE;
        }
        shmem_double_sum_to_all(&mr_sum, &mr, 1, 0, 0, v.npes, pWork, pSync);
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
    v = init_shmem();
    check_usage(v.me, v.npes, argc, argv);
    print_header_local(v.me);

    /*
     * Allocate Memory
     */
    alignment = 8;
    msg_buffer = allocate_memory(v.me, alignment);
    aligned_buffer = align_memory((unsigned long)msg_buffer, alignment);
    memset(aligned_buffer, 0, MAX_MESSAGE_SIZE * loop);

    /*
     * Time Put Message Rate
     */
    benchmark(v, aligned_buffer);

    /*
     * Finalize
     */
    shmem_free(msg_buffer);
    shmem_finalize();

    return EXIT_SUCCESS;
}
