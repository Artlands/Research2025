#!/bin/bash
#SBATCH --job-name=gups_sst       # Job name
#SBATCH --nodes=1                 # Number of nodes
#SBATCH --ntasks-per-node=32      # Number of tasks per node
#SBATCH --time=04:00:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/gups_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4        # Partition to submit to (adjust based on your system)

export OMPI_MCA_btl_base_warn_component_unused=0
export OMPI_MCA_pml="ob1"
export OMPI_MCA_btl="^openib"
export PMIX_MCA_gds="hash"

srun hostname > output32.txt

mpirun --hostfile output32.txt -n 32 sst rev-xbgas-gups-topo.py --model-options="-c sim32.cfg"

rm output32.txt


