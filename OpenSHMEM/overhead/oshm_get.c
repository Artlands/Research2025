#include <shmem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <ucs/profile/profile_on.h>

// Function to calculate elapsed time in seconds
double get_elapsed_time(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
}

int main(int argc, char **argv) {
    shmem_init();

    int me = shmem_my_pe();
    int npes = shmem_n_pes();
    // int iterations = 20000; // Number of iterations for the get operation

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
    char *send_buf = (char *) shmem_malloc(nelems);
    char *recv_buf = (char *) shmem_malloc(nelems);
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

    struct timespec start, end;
    
    if (me == 0) {
                
        // gettimeofday(&start, NULL);
        clock_gettime(CLOCK_MONOTONIC, &start);
        
        // PE 0 sends data to PE 1
        UCS_PROFILE_CODE("overhead_profile", {
            shmem_getmem(recv_buf, send_buf, nelems, 1);
        });

        // gettimeofday(&end, NULL);
        clock_gettime(CLOCK_MONOTONIC, &end);

        // total_time += get_elapsed_time(start, end);

        // printf("Start: %ld.%06ld, End: %ld.%06ld\n", start.tv_sec, start.tv_usec, end.tv_sec, end.tv_usec);

        double elapsed_us = (end.tv_sec - start.tv_sec) * 1e6 + (end.tv_nsec - start.tv_nsec) / 1e3;
        printf("Elapsed time: %.3f us\n", elapsed_us);

    }

    // if (me == 0) {
    //     double rate = (nelems / (1024.0 * 1024.0)) / total_time; // Rate in MB/s
    //     printf("Put Message Size: %zu bytes, Total Time: %.6f s, Rate: %.6f MB/s\n",
    //            nelems, total_time, rate);
    // }

    shmem_free(send_buf);
    shmem_free(recv_buf);
    shmem_finalize();
    return 0;
}