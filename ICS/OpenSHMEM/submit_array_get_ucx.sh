#!/bin/bash
#SBATCH --job-name=get_mr         # Job name
#SBATCH --nodes=2                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:5:00            # Time limit (hh:mm:ss)
#SBATCH --output=results/simple_log/sub/get_simple_ucx_%a.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=nocona        # Partition to submit to (adjust based on your system)
#SBATCH --array=0              # Array indices
#SBATCH --exclusive

# Define the message sizes (in bytes)
# MESSAGE_SIZES=(1 2 4 8 16 32 64 128 256 512 1024)
# MESSAGE_SIZES=(2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152)
MESSAGE_SIZES=(8)

# Get the message size for this array task
MESSAGE_SIZE=${MESSAGE_SIZES[$SLURM_ARRAY_TASK_ID]}

DEST_DIR=./results/simple_log/sub

export PRTE_MCA_plm_slurm_disable_warning=true

# Profile with UCX
UCX_PROFILE_MODE=log,accum UCX_PROFILE_FILE=$DEST_DIR/ucx_simple_$MESSAGE_SIZE.prof \
  oshrun -np 2 --map-by ppr:1:node ./oshm_get_simple.exe $MESSAGE_SIZE

ucx_read_profile $DEST_DIR/ucx_simple_$MESSAGE_SIZE.prof > $DEST_DIR/ucx_simple_$MESSAGE_SIZE.txt

rm -f $DEST_DIR/ucx_simple_$MESSAGE_SIZE.prof