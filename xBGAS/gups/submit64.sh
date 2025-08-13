#!/bin/bash
#SBATCH --job-name=gups_sst       # Job name
#SBATCH --nodes=1                 # Number of nodes
#SBATCH --ntasks-per-node=64       # Number of tasks per node (1 PE per node)
#SBATCH --time=04:00:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/gups_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4        # Partition to submit to (adjust based on your system)

export OMPI_MCA_btl_base_warn_component_unused=0
export OMPI_MCA_pml="ob1"
export OMPI_MCA_btl="^openib"
export PMIX_MCA_gds="hash"

srun hostname > output64.txt

mpirun --hostfile output64.txt -n 64 sst rev-xbgas-gups-topo.py --model-options="-c sim64.cfg"

rm output64.txt


