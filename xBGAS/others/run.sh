#!/bin/bash

PROGRAM="xbrtime_put_mr.exe"
OUTPUT=$(basename "$PROGRAM" .exe)

# Loop over the message sizes: 1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192
for SIZE in 1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 #16384 #65536 262144 1048576
do
    echo "Running with message size: $SIZE"
    
    # Execute the command with the current SIZE value
    sst rev-xbrtime.py -- --program "$PROGRAM" --args "$SIZE" > "./results/${OUTPUT}_${SIZE}.out"
done