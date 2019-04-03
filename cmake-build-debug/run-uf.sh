#!/usr/bin/env bash

list="16 32 64 128 256"
for uf in ${list} 
do
for alg in 0 1 2 3 4 5 6 
do
    ./run.sh ${alg} 1000000 4096 $i{uf} 0.8
    echo "==========================================================="
    done
done
