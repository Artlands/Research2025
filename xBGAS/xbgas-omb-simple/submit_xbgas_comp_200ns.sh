#!/bin/bash
#SBATCH --job-name=xbgas         # Job name
#SBATCH --nodes=1                 # Number of nodes
#SBATCH --ntasks-per-node=2       # Number of tasks per node (1 PE per node)
#SBATCH --time=01:00:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/xbgas-seq-200ns/xbgas_omb_%A_%a.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4        # Partition to submit to (adjust based on your system)
#SBATCH --array=0-1             # Array job indices (adjust based on the number of applications)

# Define an array of application executables
applications=("osu_oshm_get_seq.exe" "osu_oshm_get_seq_mr.exe")

# Select the application based on the SLURM_ARRAY_TASK_ID
app=${applications[$SLURM_ARRAY_TASK_ID]}

srun sst rev-xbgas-omb-200ns.py --model-options="$app 2"