#!/bin/bash
#SBATCH --job-name=put_mr         # Job name
#SBATCH --nodes=2                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/put_mr_%a.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=nocona        # Partition to submit to (adjust based on your system)
#SBATCH --array=0-10              # Array indices (0 to 10 for 11 message sizes)
#SBATCH --exclusive

# Load the OpenSHMEM module if required

# Define the message sizes (in bytes)
MESSAGE_SIZES=(1 4 16 64 256 1024 4096 16384 65536 262144 1048576)

# Get the message size for this array task
MESSAGE_SIZE=${MESSAGE_SIZES[$SLURM_ARRAY_TASK_ID]}

export PRTE_MCA_plm_slurm_disable_warning=true

# Run the benchmark using oshrun
oshrun -np 2 --map-by ppr:1:node ./oshm_put_mr.exe $MESSAGE_SIZE