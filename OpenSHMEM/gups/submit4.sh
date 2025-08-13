#!/bin/bash
#SBATCH --job-name=gups           # Job name
#SBATCH --nodes=4                 # Number of nodes
#SBATCH --ntasks-per-node=1       # Number of tasks per node (1 PE per node)
#SBATCH --time=00:10:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/gups_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4        # Partition to submit to (adjust based on your system)

srun hostname > output4.txt

mpirun --hostfile output4.txt -n 4 ./gups_raw.exe -m 65536 -n 10000

rm output4.txt