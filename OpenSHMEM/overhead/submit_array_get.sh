#!/bin/bash
#SBATCH --job-name=get                                 # Job name
#SBATCH --nodes=2                                      # Number of nodes
#SBATCH --ntasks-per-node=1                            # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00                                # Time limit (hh:mm:ss)
#SBATCH --output=results/get_%a.out    # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4                               # Partition to submit to (adjust based on your system)
#SBATCH --array=0-11                                    # Array indices
#SBATCH --exclusive


export UCX_NET_DEVICES=ib0


# Define the message sizes (in bytes)
MESSAGE_SIZES=( 1 4 16 64 256 1024 4096 16384 65536 262144 1048576 4194304 ) # 

# Get the message size for this array task
MESSAGE_SIZE=${MESSAGE_SIZES[$SLURM_ARRAY_TASK_ID]}

if [ "$SLURM_ARRAY_TASK_ID" -ge "${#MESSAGE_SIZES[@]}" ]; then
    echo "Invalid SLURM_ARRAY_TASK_ID=$SLURM_ARRAY_TASK_ID" >&2
    exit 1
fi

DEST_DIR=/mnt/REPACSS/home/li29729/Research2025/OpenSHMEM/overhead/results

mkdir -p "$DEST_DIR"

# Profile with UCX
UCX_PROFILE_MODE=log,accum UCX_PROFILE_FILE=$DEST_DIR/ucx_$MESSAGE_SIZE.prof \
  oshrun -np 2 --map-by ppr:1:node /mnt/REPACSS/home/li29729/Research2025/OpenSHMEM/overhead/oshm_get.exe $MESSAGE_SIZE

ucx_read_profile $DEST_DIR/ucx_$MESSAGE_SIZE.prof > $DEST_DIR/ucx_$MESSAGE_SIZE.txt

rm -f $DEST_DIR/ucx_$MESSAGE_SIZE.prof