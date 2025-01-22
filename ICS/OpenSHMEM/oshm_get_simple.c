#include <shmem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

// Function to calculate elapsed time in seconds
double get_elapsed_time(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
}

int main(int argc, char **argv) {
    shmem_init();

    int me = shmem_my_pe();
    int npes = shmem_n_pes();

    if (npes < 2) {
        if (me == 0) {
            fprintf(stderr, "This benchmark requires at least two PEs.\n");
        }
        shmem_finalize();
        return 1;
    }

    if (argc != 2) {
        if (me == 0) {
            fprintf(stderr, "Usage: %s <message_size_in_bytes>\n", argv[0]);
        }
        shmem_finalize();
        return 1;
    }

    size_t nelems = atol(argv[1]); // Message size in bytes

    // Allocate symmetric memory
    char *send_buf = shmem_malloc(nelems);
    char *recv_buf = shmem_malloc(nelems);
    if (!send_buf || !recv_buf) {
        if (me == 0) {
            fprintf(stderr, "Failed to allocate symmetric memory.\n");
        }
        shmem_finalize();
        return 1;
    }

    // Initialize send and send/receive buffers
    memset(send_buf, 'a', nelems);
    memset(recv_buf, 0, nelems);

    double total_time = 0.0;

    shmem_barrier_all(); // Ensure synchronization

    struct timeval start, end;
    if (me == 0) {
        gettimeofday(&start, NULL);

        // PE 0 sends data to PE 1
        shmem_char_get(recv_buf, send_buf, nelems, 1);

        gettimeofday(&end, NULL);

        total_time += get_elapsed_time(start, end);
    }

    shmem_barrier_all(); // Ensure synchronization

    if (me == 0) {
        double rate = (nelems / (1024.0 * 1024.0)) / total_time; // Rate in MB/s
        printf("Put Message Size: %zu bytes, Total Time: %.6f s, Rate: %.6f MB/s\n",
               nelems, total_time, rate);
    }

    shmem_free(send_buf);
    shmem_free(recv_buf);
    shmem_finalize();
    return 0;
}