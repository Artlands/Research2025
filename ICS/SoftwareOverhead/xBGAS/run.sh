#!/bin/bash

PROGRAM="xbrtime_put_mr.exe"
OUTPUT=$(basename "$PROGRAM" .exe)

# Loop over the message sizes: 1, 4, 16, ..., 1048576
for SIZE in 1 4 16 64 256 1024 4096 16384 #65536 262144 1048576
do
    echo "Running with message size: $SIZE"
    
    # Execute the command with the current SIZE value
    sst rev-xbrtime.py -- --program "$PROGRAM" --args "$SIZE" > "./results/${OUTPUT}_${SIZE}.out"
done