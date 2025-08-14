#!/bin/bash
#SBATCH --job-name=gups       # Job name
#SBATCH --nodes=1                 # Number of nodes
#SBATCH --ntasks-per-node=16       # Number of tasks per node (1 PE per node)
#SBATCH --time=01:00:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/gups_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4        # Partition to submit to (adjust based on your system)

srun sst ../rev-xbgas-gups-topo.py --model-options="-c ../config_200ns/sim16.cfg"

