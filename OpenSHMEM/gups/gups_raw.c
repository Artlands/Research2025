/* -*- mode: C; tab-width: 2; indent-tabs-mode: nil; -*- */

/*
* This code has been contributed by the DARPA HPCS program.
* [Additional header comments omitted for brevity]
*/

#include <stdio.h>
#include <shmem.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>
#if 0
#include "config.h"
#endif

/* Random number generator constants */
#define POLY 0x0000000000000007UL
#define PERIOD 1317624576693539401L

/* Define 64-bit constant */
#define ZERO64B 0LL

/* Global options and variables */
uint64_t TotalMemOpt = 8192;
uint64_t NumUpdatesOpt = 0;
double SHMEMGUPs;
double SHMEMRandomAccess_ErrorsFraction;
double SHMEMRandomAccess_time;
double SHMEMRandomAccess_CheckTime;
int Failure;

/* Global pointers allocated in symmetric memory */
uint64_t *HPCC_Table;
long *HPCC_PELock;

/* Global starting index for this PE */
static uint64_t GlobalStartMyProc;

/* Function prototype */
int SHMEMRandomAccess(void);

static double RTSEC(void)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec + tp.tv_usec / 1.0e6;
}

static void print_usage(void)
{
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, " %-20s %s\n", "-h", "display this help message");
    fprintf(stderr, " %-20s %s\n", "-m", "memory in bytes per PE");
    fprintf(stderr, " %-20s %s\n", "-n", "number of updates per PE");
    return;
}

static int64_t starts(uint64_t n)
{
    int i, j;
    uint64_t m2[64];
    uint64_t temp, ran;

    while (n > PERIOD)
        n -= PERIOD;
    if (n == 0)
        return 0x1;

    temp = 0x1;
    for (i = 0; i < 64; i++) {
        m2[i] = temp;
        temp = (temp << 1) ^ ((int64_t)temp < 0 ? POLY : 0);
        temp = (temp << 1) ^ ((int64_t)temp < 0 ? POLY : 0);
    }

    for (i = 62; i >= 0; i--)
        if ((n >> i) & 1)
            break;

    ran = 0x2;
    while (i > 0) {
        temp = 0;
        for (j = 0; j < 64; j++)
            if ((ran >> j) & 1)
                temp ^= m2[j];
        ran = temp;
        i -= 1;
        if ((n >> i) & 1)
            ran = (ran << 1) ^ ((int64_t)ran < 0 ? POLY : 0);
    }
    return ran;
}

static void UpdateTable(uint64_t *Table,
                        uint64_t TableSize,
                        uint64_t MinLocalTableSize,
                        uint64_t Top,
                        int Remainder,
                        uint64_t niterate,
                        int use_lock)
{
    uint64_t iterate;
    int index;
    uint64_t ran, global_offset;
    int remote_pe;
    int global_start_at_pe;
#ifdef USE_GET_PUT
    uint64_t remote_val;
#endif

    /* Initialize random seed */
    ran = starts(4 * GlobalStartMyProc);

    for (iterate = 0; iterate < niterate; iterate++) {
        ran = (ran << 1) ^ ((int64_t)ran < ZERO64B ? POLY : ZERO64B);
        global_offset = ran & (TableSize - 1);
        if (global_offset < Top) {
            remote_pe = global_offset / (MinLocalTableSize + 1);
            global_start_at_pe = (MinLocalTableSize + 1) * remote_pe;
        } else {
            remote_pe = (global_offset - Remainder) / MinLocalTableSize;
            global_start_at_pe = MinLocalTableSize * remote_pe + Remainder;
        }
        index = global_offset - global_start_at_pe;

        if (use_lock)
            shmem_set_lock(&HPCC_PELock[remote_pe]);
#ifdef USE_GET_PUT
        remote_val = (uint64_t) shmem_long_g((long *)&Table[index], remote_pe);
        remote_val ^= ran;
        shmem_long_p((long *)&Table[index], remote_val, remote_pe);
#else
        shmem_uint64_atomic_xor(&Table[index], ran, remote_pe);
#endif
        if (use_lock)
            shmem_clear_lock(&HPCC_PELock[remote_pe]);
    }
}

int SHMEMRandomAccess(void)
{
    int64_t i;
    uint64_t i_u;
    /* Declare reduction variables as long long to match shmem_longlong_sum_to_all */
    static long long NumErrors = 0, GlbNumErrors = 0;

    int NumProcs, MyProc;
    int Remainder;            /* Number of processors with (LocalTableSize + 1) entries */
    uint64_t Top;             /* Number of table entries in top of Table */
    uint64_t LocalTableSize;  /* Local table width */
    uint64_t MinLocalTableSize; /* Integer ratio TableSize/NumProcs */
    uint64_t logTableSize, TableSize;
    double RealTime;
    double TotalMem;
    static int sAbort = 0, rAbort = 0;
    uint64_t NumUpdates;      /* total number of updates to table */
    uint64_t ProcNumUpdates;  /* number of updates per processor */
    FILE *outFile = NULL;
    double *GUPs;
    double *temp_GUPs;

    SHMEMGUPs = -1;
    GUPs = &SHMEMGUPs;

    NumProcs = shmem_n_pes();
    MyProc = shmem_my_pe();

    if (MyProc == 0) {
        outFile = stdout;
        setbuf(outFile, NULL);
    }

    /* Calculate memory and table size */
    TotalMem = TotalMemOpt;         /* max single node memory */
    TotalMem *= NumProcs;           /* max memory in NumProcs nodes */
    TotalMem /= sizeof(uint64_t);

    for (TotalMem *= 0.5, logTableSize = 0, TableSize = 1;
         TotalMem >= 1.0;
         TotalMem *= 0.5, logTableSize++, TableSize <<= 1)
        ; /* EMPTY */

    MinLocalTableSize = TableSize / NumProcs;
    Remainder = TableSize - (MinLocalTableSize * NumProcs);
    Top = (MinLocalTableSize + 1) * Remainder;
    if (MyProc < Remainder) {
        LocalTableSize = MinLocalTableSize + 1;
        GlobalStartMyProc = (MinLocalTableSize + 1) * MyProc;
    } else {
        LocalTableSize = MinLocalTableSize;
        GlobalStartMyProc = (MinLocalTableSize * MyProc) + Remainder;
    }

    sAbort = 0;
    HPCC_Table = shmem_malloc((Remainder > 0 ? (MinLocalTableSize + 1) : LocalTableSize)
                              * sizeof(uint64_t));
    if (!HPCC_Table)
        sAbort = 1;

    HPCC_PELock = (long *)shmem_malloc(sizeof(long) * NumProcs);
    if (!HPCC_PELock)
        sAbort = 1;

    shmem_barrier_all();

    /* Use older API for integer reduction */
    static int pwrk_int[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
    static long pSync_int[SHMEM_REDUCE_SYNC_SIZE];
    for (int i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i++) {
        pSync_int[i] = SHMEM_SYNC_VALUE;
    }
    shmem_int_sum_to_all(&rAbort, &sAbort, 1, 0, 0, shmem_n_pes(), pwrk_int, pSync_int);
    shmem_barrier_all();

    if (rAbort > 0) {
        if (MyProc == 0)
            fprintf(outFile, "Failed to allocate memory\n");
        if (HPCC_Table)
            shmem_free(HPCC_Table);
        if (HPCC_PELock)
            shmem_free(HPCC_PELock);
        goto failed_table;
    }

    for (i = 0; i < NumProcs; i++)
        HPCC_PELock[i] = 0;

    if (NumUpdatesOpt == 0) {
        ProcNumUpdates = 4 * LocalTableSize;
        NumUpdates = 4 * TableSize;
    } else {
        ProcNumUpdates = NumUpdatesOpt;
        NumUpdates = NumUpdatesOpt * NumProcs;
    }

    if (MyProc == 0) {
        fprintf(outFile, "Running on %d processors\n", NumProcs);
        fprintf(outFile, "Total Main table size = 2^%" PRIu64 " = %" PRIu64 " words\n",
                logTableSize, TableSize);
        fprintf(outFile, "PE Main table size = (2^%" PRIu64 ")/%d  = %" PRIu64 " words/PE MAX\n",
                logTableSize, NumProcs, LocalTableSize);
        fprintf(outFile, "Total number of updates = %" PRIu64 "\n", NumUpdates);
    }

    for (i_u = 0; i_u < LocalTableSize; i_u++)
        HPCC_Table[i_u] = i_u + GlobalStartMyProc;

    shmem_barrier_all();

    RealTime = -RTSEC();
    shmem_barrier_all();
    
    UpdateTable(HPCC_Table,
                TableSize,
                MinLocalTableSize,
                Top,
                Remainder,
                ProcNumUpdates,
                0);

    shmem_barrier_all();
    RealTime += RTSEC();

    if (MyProc == 0) {
        SHMEMRandomAccess_time = RealTime;
        *GUPs = 1e-9 * NumUpdates / RealTime;
        fprintf(outFile, "Real time used = %.6f seconds\n", RealTime);
        fprintf(outFile, "%.9f Billion(10^9) Updates    per second [GUP/s]\n", *GUPs);
        fprintf(outFile, "%.9f Billion(10^9) Updates/PE per second [GUP/s]\n", *GUPs / NumProcs);
    }
    temp_GUPs = GUPs;

    /* Use older broadcast for double values via shmem_broadcast64.
       Note: The call requires 8 arguments:
         target, source, nlong, PE_root, PE_start, logPE_stride, PE_size, pSync */
    static long pSync_bcast[SHMEM_BCAST_SYNC_SIZE];
    for (int i = 0; i < SHMEM_BCAST_SYNC_SIZE; i++) {
        pSync_bcast[i] = SHMEM_SYNC_VALUE;
    }
    shmem_barrier_all();
    shmem_broadcast64(GUPs, temp_GUPs, 1, 0, 0, 0, shmem_n_pes(), pSync_bcast);
    shmem_barrier_all();

    RealTime = -RTSEC();
    shmem_barrier_all();
    UpdateTable(HPCC_Table,
                TableSize,
                MinLocalTableSize,
                Top,
                Remainder,
                ProcNumUpdates,
                1);
    shmem_barrier_all();

    NumErrors = 0;
    for (i_u = 0; i_u < LocalTableSize; i_u++) {
        if (HPCC_Table[i_u] != i_u + GlobalStartMyProc)
            NumErrors++;
    }

    /* Use older API for long long reduction.
       Note: pwrk_ll and pSync_ll are declared only once here. */
    static long long pwrk_ll[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
    static long pSync_ll[SHMEM_REDUCE_SYNC_SIZE];
    for (int i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i++) {
        pSync_ll[i] = SHMEM_SYNC_VALUE;
    }
    shmem_barrier_all();
    shmem_longlong_sum_to_all(&GlbNumErrors, &NumErrors, 1, 0, 0, shmem_n_pes(), pwrk_ll, pSync_ll);
    shmem_barrier_all();
    RealTime += RTSEC();

    if (MyProc == 0) {
        fprintf(outFile, "Verification:  Real time used = %.6f seconds\n", RealTime);
        fprintf(outFile, "Found %" PRIu64 " errors in %" PRIu64 " locations (%s).\n",
                (uint64_t)GlbNumErrors, TableSize,
                (GlbNumErrors <= 0.01 * TableSize) ? "passed" : "failed");
        if (GlbNumErrors > 0.01 * TableSize)
            Failure = 1;
    }

    shmem_free(HPCC_Table);
    shmem_free(HPCC_PELock);
failed_table:
    if (MyProc == 0 && outFile != stderr)
        fclose(outFile);

    shmem_barrier_all();
    return 0;
}

int main(int argc, char **argv)
{
    int op;
    while ((op = getopt(argc, argv, "hm:n:")) != -1) {
        switch (op) {
            case 'm':
                TotalMemOpt = atoll(optarg);
                if (TotalMemOpt <= 0) {
                    print_usage();
                    return -1;
                }
                break;
            case 'n':
                NumUpdatesOpt = atoi(optarg);
                if (NumUpdatesOpt <= 0) {
                    print_usage();
                    return -1;
                }
                break;
            case '?':
            case 'h':
                print_usage();
                return -1;
        }
    }
    shmem_init();
    SHMEMRandomAccess();
    shmem_finalize();
    return 0;
}