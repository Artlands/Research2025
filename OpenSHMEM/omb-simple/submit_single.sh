#!/bin/bash
#SBATCH --job-name=omb         # Job name
#SBATCH --nodes=2                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/test/omb_%A_%a.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4        # Partition to submit to (adjust based on your system)
#SBATCH --exclusive

module load openmpi

applications="osu_oshm_put.exe"

srun ./$applications
