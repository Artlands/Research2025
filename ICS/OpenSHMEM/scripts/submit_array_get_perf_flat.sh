#!/bin/bash
#SBATCH --job-name=get_mr         # Job name
#SBATCH --nodes=2                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/7round/get_mr_%a.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=nocona        # Partition to submit to (adjust based on your system)
#SBATCH --array=0-15              # Array indices (0 to 11 for 12 message sizes)
#SBATCH --exclusive

# Define the message sizes (in bytes)
MESSAGE_SIZES=('1B' '4B' '16B' '64B' '256B' '1KB' '4KB' '16KB' '64KB' '256KB' '512KB' '1MB' '4MB' '16MB' '64MB' '256MB')

# Get the message size for this array task
MESSAGE_SIZE=${MESSAGE_SIZES[$SLURM_ARRAY_TASK_ID]}

export PRTE_MCA_plm_slurm_disable_warning=true

# Run the benchmark using oshrun
perf record --freq=max -o results/7round/perf_data_${MESSAGE_SIZE}.data \
    oshrun -np 2 --map-by ppr:1:node ./oshm_get_mr_$MESSAGE_SIZE.exe

perf report -i results/7round/perf_data_${MESSAGE_SIZE}.data > results/7round/perf_report_${MESSAGE_SIZE}.txt

rm -f results/7round/perf_data_${MESSAGE_SIZE}.data

# perf script -i results/perf/perf_data_${MESSAGE_SIZE}.data | ./FlameGraph/stackcollapse-perf.pl > results/flames/flame_${MESSAGE_SIZE}.folded

# ./FlameGraph/flamegraph.pl results/flames/flame_${MESSAGE_SIZE}.folded > results/flames/flame_${MESSAGE_SIZE}.svg