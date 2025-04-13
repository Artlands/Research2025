#!/bin/bash
#SBATCH --job-name=ct_sst       # Job name
#SBATCH --nodes=1                 # Number of nodes
#SBATCH --ntasks-per-node=8       # Number of tasks per node (1 PE per node)
#SBATCH --time=01:00:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/ct_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=nocona        # Partition to submit to (adjust based on your system)

export OMPI_MCA_btl_base_warn_component_unused=0
export OMPI_MCA_pml="ob1"
export OMPI_MCA_btl="^openib"
export PMIX_MCA_gds="hash"

srun hostname > output8.txt

mpirun --hostfile output8.txt -n 8 sst rev-xbgas-ct-topo.py --model-options="-c sim8.cfg"

rm output8.txt

