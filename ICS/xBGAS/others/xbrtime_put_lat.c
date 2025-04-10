#include "xbrtime.h"

double get_microsecond_time()
{
  double retval;
  struct __kernel_timespec t;
  rev_clock_gettime( 0, &t );
  retval = ((double)t.tv_nsec/1000);
  return retval;
}

// Function to calculate elapsed time in microseconds
double get_elapsed_time_micro(double start, double end) {
    return (end - start);
}

int main(int argc, char **argv) {
    xbrtime_init();

    int me = xbrtime_mype();
    int npes = xbrtime_num_pes();
    
    // if (me == 0)
    //     printf( "Number of PEs: %d\n", npes);

    if (npes < 2) {
        if (me == 0) {
            printf("This benchmark requires at least two PEs.\n");
        }
        xbrtime_close();
        return 1;
    }

    if (argc != 2) {
        if (me == 0) {
            printf("Usage: %s <message_size_in_bytes>\n", argv[0]);
        }
        xbrtime_close();
        return 1;
    }

    size_t msg_size = atol(argv[1]); // Message size in bytes
    int runs = 5; // Number of runs for averaging

    // Allocate symmetric memory
    char *send_buf = (char *)xbrtime_align(8, msg_size);
    char *recv_buf = (char *)xbrtime_align(8, msg_size);
    if (!send_buf || !recv_buf) {
        if (me == 0) {
            printf("Failed to allocate symmetric memory.\n");
        }
        xbrtime_close();
        return 1;
    }

    // Initialize send and recv buffer with random data
    for (size_t i = 0; i < msg_size; i++) {
        send_buf[i] = 'a';
        recv_buf[i] = 0;
    }

    double total_time = 0.0;

    for (int r = 0; r < runs; r++) {
        xbrtime_barrier_all(); // Ensure synchronization

        double start, end;
        if (me == 0) {
            start = get_microsecond_time();

            // PE 0 puts data to PE 1
            xbrtime_putmem(recv_buf, send_buf, msg_size, 1);

            // Wait for completion
            xbrtime_quiet();

            end = get_microsecond_time();

            total_time += get_elapsed_time_micro(start, end);
        }

        xbrtime_barrier_all(); // Ensure synchronization
    }

    if (me == 0) {
        double avg_time = total_time / runs; // Average time 
        printf("Put Message Size: %d bytes, Average Time: %.4f us\n",
               msg_size, avg_time);
    }

    xbrtime_free(send_buf);
    xbrtime_free(recv_buf);
    xbrtime_close();
    return 0;
}