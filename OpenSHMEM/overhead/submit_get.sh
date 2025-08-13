#!/bin/bash
#SBATCH --job-name=get                                 # Job name
#SBATCH --nodes=2                                      # Number of nodes
#SBATCH --ntasks-per-node=1                            # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00                                # Time limit (hh:mm:ss)
#SBATCH --output=results/get_4.out    # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4                               # Partition to submit to (adjust based on your system)
#SBATCH --exclusive

# module load openmpi
# spack load /lhkrdfy

export UCX_NET_DEVICES=ib0


# Get the message size for this array task
MESSAGE_SIZE=4194304

DEST_DIR=/mnt/REPACSS/home/li29729/Research2025/OpenSHMEM/overhead2/results

mkdir -p "$DEST_DIR"

# Profile with UCX
UCX_PROFILE_MODE=log,accum UCX_PROFILE_FILE=$DEST_DIR/ucx_$MESSAGE_SIZE.prof \
  srun /mnt/REPACSS/home/li29729/Research2025/OpenSHMEM/overhead2/oshm_get.exe $MESSAGE_SIZE

ucx_read_profile $DEST_DIR/ucx_$MESSAGE_SIZE.prof > $DEST_DIR/ucx_$MESSAGE_SIZE.txt

rm -f $DEST_DIR/ucx_$MESSAGE_SIZE.prof