#!/bin/bash
#SBATCH --job-name=gups           # Job name
#SBATCH --nodes=2                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/gups_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=nocona        # Partition to submit to (adjust based on your system)

srun hostname > output.txt

mpirun --hostfile output.txt -n 2 ./gups_raw.exe -m 65536 -n 10000

rm output.txt