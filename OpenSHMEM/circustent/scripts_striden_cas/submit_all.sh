#!/bin/bash
# submit_all.sh
# Submit batch jobs for different sizes

# List of suffixes
sizes=( "" 4 8 16 32 64 128 256 )

for s in "${sizes[@]}"; do
    if [ -z "$s" ]; then
        script="submit.sh"
    else
        script="submit${s}.sh"
    fi

    if [[ -f "$script" ]]; then
        sbatch "$script"
    else
        echo "Warning: $script not found."
    fi
done
