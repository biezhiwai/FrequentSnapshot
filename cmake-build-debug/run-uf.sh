#!/usr/bin/env bash

#uf="16 32 64 128 256"
uf=256
for j in ${uf} 
do
for i in 0 1 2 3 4 5 6 
do
    ./run.sh $i 250000 4096 $j 0.8
    echo "==========================================================="
    done
done
