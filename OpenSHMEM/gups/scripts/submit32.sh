#!/bin/bash
#SBATCH --job-name=gups           # Job name
#SBATCH --nodes=32                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/gups_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4        # Partition to submit to (adjust based on your system)
#SBATCH --exclusive

srun ../gups.exe -m 65536 -n 10000

