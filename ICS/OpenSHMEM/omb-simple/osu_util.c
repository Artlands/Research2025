#include "osu_util.h"

double getMicrosecondTimeStamp()
{
    double retval;
    struct timeval t;
    gettimeofday(&t, NULL);
    retval = (double) t.tv_sec * 1e6 + (double) t.tv_usec;
    return retval;
}

void print_header_pgas(const char *header, int rank, int full)
{
    if (rank == 0) {
        printf(header, "");

        printf("%-*s", 10, "# Size");
        printf("%*s", FIELD_WIDTH, "Avg Latency(us)");
        printf("%*s", FIELD_WIDTH, "Min Latency(us)");
        printf("%*s", FIELD_WIDTH, "Max Latency(us)");
        printf("%*s\n", 12, "Iterations");
    }
}

void print_data_pgas(int rank, int full, int size, double avg_time,
                     double min_time, double max_time, int iterations)
{
    if (rank == 0) {
        if (size) {
            printf("%-*d", 10, size);
            printf("%*.*f", FIELD_WIDTH, FLOAT_PRECISION, avg_time);
        }

        else {
            printf("%*.*f", 17, FLOAT_PRECISION, avg_time);
        }

        if (full) {
            printf("%*.*f%*.*f%*d\n", FIELD_WIDTH, FLOAT_PRECISION,
                    min_time, FIELD_WIDTH, FLOAT_PRECISION, max_time, 12,
                    iterations);
        }

        else {
            printf("\n");
        }
    }
}