#!/bin/bash
#SBATCH --job-name=get_mr         # Job name
#SBATCH --nodes=2                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/detail_large/get_mr_%a.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=nocona        # Partition to submit to (adjust based on your system)
#SBATCH --array=0-9              # Array indices (0 to 10 for 11 message sizes)
#SBATCH --exclusive

# Load the OpenSHMEM module if required

# Define the message sizes (in bytes)
MESSAGE_SIZES=(16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608)

# Get the message size for this array task
MESSAGE_SIZE=${MESSAGE_SIZES[$SLURM_ARRAY_TASK_ID]}

export PRTE_MCA_plm_slurm_disable_warning=true

# Run the benchmark using oshrun
perf record -g -o results/perf_large/perf_data_${SLURM_ARRAY_TASK_ID}.data \
    oshrun -np 2 --map-by ppr:1:node ./oshm_get_mr.exe $MESSAGE_SIZE

perf report -i results/perf_large/perf_data_${SLURM_ARRAY_TASK_ID}.data > results/perf_large/perf_report_${SLURM_ARRAY_TASK_ID}.txt

perf script -i results/perf_large/perf_data_${SLURM_ARRAY_TASK_ID}.data | ./FlameGraph/stackcollapse-perf.pl > results/flames_large/flame_${SLURM_ARRAY_TASK_ID}.folded

./FlameGraph/flamegraph.pl results/flames_large/flame_${SLURM_ARRAY_TASK_ID}.folded > results/flames_large/flame_${SLURM_ARRAY_TASK_ID}.svg