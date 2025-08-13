#!/bin/bash
#SBATCH --job-name=gups_sst       # Job name
#SBATCH --nodes=2                 # Number of nodes
#SBATCH --ntasks-per-node=128       # Number of tasks per node (1 PE per node)
#SBATCH --time=08:00:00           # Time limit (hh:mm:ss)
#SBATCH --output=results/gups_%A.out     # Standard output log (%j expands to job ID)
#SBATCH --partition=zen4        # Partition to submit to (adjust based on your system)

export OMPI_MCA_btl_base_warn_component_unused=0
export OMPI_MCA_pml="ob1"
export OMPI_MCA_btl="^openib"
export PMIX_MCA_gds="hash"

srun hostname > output256.txt

mpirun --hostfile output256.txt -n 256 sst rev-xbgas-gups-topo.py --model-options="-c sim256.cfg"

rm output256.txt


