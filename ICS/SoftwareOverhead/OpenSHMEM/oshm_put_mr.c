#include <shmem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

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

    size_t msg_size = atol(argv[1]); // Message size in bytes
    int runs = 5; // Number of runs for averaging

    // Allocate symmetric memory
    char *send_buf = shmem_malloc(msg_size);
    char *recv_buf = shmem_malloc(msg_size);
    if (!send_buf || !recv_buf) {
        if (me == 0) {
            fprintf(stderr, "Failed to allocate symmetric memory.\n");
        }
        shmem_finalize();
        return 1;
    }

    // Initialize send and recv buffer with random data
    srand(time(NULL));
    for (size_t i = 0; i < msg_size; i++) {
        send_buf[i] = rand() % 256;
        recv_buf[i] = 0;
    }

    double total_time = 0.0;

    for (int r = 0; r < runs; r++) {
        shmem_barrier_all(); // Ensure synchronization

        struct timeval start, end;
        if (me == 0) {
            gettimeofday(&start, NULL);

            // PE 0 sends data to PE 1
            shmem_putmem(recv_buf, send_buf, msg_size, 1);

            // Wait for completion
            shmem_quiet();

            gettimeofday(&end, NULL);

            total_time += get_elapsed_time(start, end);
        }

        shmem_barrier_all(); // Ensure synchronization
    }

    if (me == 0) {
        double avg_time = total_time / runs; // Average time per run
        double rate = (msg_size / (1024.0 * 1024.0)) / avg_time; // Rate in MB/s
        printf("Message Size: %zu bytes, Average Time: %.6f s, Rate: %.2f MB/s\n",
               msg_size, avg_time, rate);
    }

    shmem_free(send_buf);
    shmem_free(recv_buf);
    shmem_finalize();
    return 0;
}