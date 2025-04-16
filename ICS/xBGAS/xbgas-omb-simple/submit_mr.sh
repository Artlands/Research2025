#!/bin/bash
#SBATCH --job-name=omb_seq     # Job name
#SBATCH --nodes=1                 # Number of nodes
#SBATCH --ntasks-per-node=2       # Number of tasks per node (1 PE per node)
#SBATCH --time=12:00:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/get_seq_mr_100ns_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=nocona        # Partition to submit to (adjust based on your system)

export OMPI_MCA_btl_base_warn_component_unused=0
export OMPI_MCA_pml="ob1"
export OMPI_MCA_btl="^openib"
export PMIX_MCA_gds="hash"

srun hostname > output.txt

mpirun --hostfile output.txt -n 1 sst -n 2 rev-xbgas-omb.py --model-options="osu_oshm_get_seq_mr.exe 2"

rm output.txt