#!/bin/bash
#SBATCH --job-name=omb         # Job name
#SBATCH --nodes=2                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/omb_update_%A_%a.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=nocona        # Partition to submit to (adjust based on your system)
#SBATCH --exclusive
#SBATCH --array=0-7             # Array job indices (adjust based on the number of applications)

# Define an array of application executables
applications=("osu_oshm_get.exe" "osu_oshm_put.exe" "osu_oshm_get_nb.exe" "osu_oshm_put_nb.exe" "osu_oshm_get_mr.exe" "osu_oshm_put_mr.exe" "osu_oshm_get_mr_nb.exe" "osu_oshm_put_mr_nb.exe")

# Select the application based on the SLURM_ARRAY_TASK_ID
app=${applications[$SLURM_ARRAY_TASK_ID]}

oshrun -np 2 --map-by ppr:1:node ./$app
