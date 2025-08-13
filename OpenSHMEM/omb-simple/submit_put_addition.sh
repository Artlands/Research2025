#!/bin/bash
#SBATCH --job-name=omb         # Job name
#SBATCH --nodes=2                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/omb_update_add_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4        # Partition to submit to (adjust based on your system)
#SBATCH --exclusive

oshrun -np 2 --map-by ppr:1:node ./osu_oshm_put_mr.exe
