#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <stddef.h>
#include <unistd.h>

#ifndef XBGAS_OSHM_UTIL_H
#define XBGAS_OSHM_UTIL_H

#define TIME() getMicrosecondTimeStamp()
double getMicrosecondTimeStamp();

void print_header_pgas(const char *header, int rank, int full);
void print_data_pgas(int rank, int full, int size, double avg_time,
                     double min_time, double max_time, int iterations);

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef BENCHMARK
#define BENCHMARK "OSU-MICRO BENCHMARK"
#endif

#define HEADER "# " BENCHMARK "\n"

#ifndef FIELD_WIDTH
#define FIELD_WIDTH 20
#endif

#ifndef FLOAT_PRECISION
#define FLOAT_PRECISION 3
#endif

#define max(a, b) (a > b ? a : b)

#define MESSAGE_ALIGNMENT               64
#define MESSAGE_ALIGNMENT_MR            (1 << 12)
#define MAX_MESSAGE_SIZE                (1 << 22)
#define MAX_MSG_SIZE_PT2PT              (1 << 21)
#define LARGE_MESSAGE_SIZE              8192
#define OSHM_LOOP_SMALL_MR              500
#define OSHM_LOOP_LARGE_MR              50
#define OSHM_LOOP_SMALL                 1000
#define OSHM_LOOP_LARGE                 100
#define OSHM_SKIP_SMALL                 200
#define OSHM_SKIP_LARGE                 10
#define OSHM_LOOP_ATOMIC                500

#define MYBUFSIZE                       MAX_MESSAGE_SIZE

#define MYBUFSIZE_MR                                                           \
    (MAX_MESSAGE_SIZE * OSHM_LOOP_LARGE_MR + MESSAGE_ALIGNMENT_MR)

#endif