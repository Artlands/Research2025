#!/bin/bash
#SBATCH --job-name=circustent     # Job name
#SBATCH --nodes=32                 # Number of nodes
#SBATCH --ntasks-per-node=8       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/circustent_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4        # Partition to submit to (adjust based on your system)
#SBATCH --exclusive

srun $HOME/local/circustent/bin/circustent -b RAND_ADD -m 16384 -p 256 -i 1000