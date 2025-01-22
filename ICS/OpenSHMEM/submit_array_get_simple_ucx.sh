#!/bin/bash
#SBATCH --job-name=get_mr         # Job name
#SBATCH --nodes=2                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/get_mr_ucx_simple_%a.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=nocona        # Partition to submit to (adjust based on your system)
#SBATCH --array=0-15              # Array indices
#SBATCH --exclusive

# Define the message sizes (in bytes)
MESSAGE_SIZES=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768)

# Get the message size for this array task
MESSAGE_SIZE=${MESSAGE_SIZES[$SLURM_ARRAY_TASK_ID]}

export PRTE_MCA_plm_slurm_disable_warning=true

# Profile with UCX
UCX_PROFILE_MODE=log,accum UCX_PROFILE_FILE=./results/ucx_simple_$MESSAGE_SIZE.prof \
  oshrun -np 2 --map-by ppr:1:node ./oshm_get_mr.exe $MESSAGE_SIZE

ucx_read_profile ./results/ucx_simple_$MESSAGE_SIZE.prof > ./results/ucx_simple_$MESSAGE_SIZE.txt

rm -f ./results/ucx_simple_$MESSAGE_SIZE.prof