#!/usr/bin/env bash

list="16 32 64 128 256"
for uf in ${list} 
do
    ./run.sh 1000000 4096 ${uf}
    echo "==========================================================="
done
