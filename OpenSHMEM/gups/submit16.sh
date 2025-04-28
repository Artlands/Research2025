#!/bin/bash
#SBATCH --job-name=gups           # Job name
#SBATCH --nodes=16                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/gups_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=nocona        # Partition to submit to (adjust based on your system)

srun hostname > output16.txt

mpirun --hostfile output16.txt -n 16 ./gups_raw.exe -m 65536 -n 10000

rm output16.txt

