#!/bin/bash
#SBATCH --job-name=get_mr         # Job name
#SBATCH --nodes=2                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/get_mr_perf_%a.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=nocona        # Partition to submit to (adjust based on your system)
#SBATCH --array=0-15              # Array indices
#SBATCH --exclusive

# Define the message sizes (in bytes)
MESSAGE_SIZES=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768)

# Get the message size for this array task
MESSAGE_SIZE=${MESSAGE_SIZES[$SLURM_ARRAY_TASK_ID]}

export PRTE_MCA_plm_slurm_disable_warning=true

# Profile with perf
perf record --freq=max -o results/perf_data_${MESSAGE_SIZE}.data \
  oshrun -np 2 --map-by ppr:1:node ./oshm_get_mr.exe $MESSAGE_SIZE

perf report -i results/perf_data_${MESSAGE_SIZE}.data > results/perf_${MESSAGE_SIZE}.txt

rm -f results/perf_data_${MESSAGE_SIZE}.data
