#!/bin/bash
#SBATCH --job-name=ct_sst       # Job name
#SBATCH --nodes=1                 # Number of nodes
#SBATCH --ntasks-per-node=4       # Number of tasks per node (1 PE per node)
#SBATCH --time=01:00:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/ct_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4        # Partition to submit to (adjust based on your system)

srun sst ../rev-xbgas-ct-topo.py --model-options="-c ../config_rand_add_100ns/sim4.cfg"
