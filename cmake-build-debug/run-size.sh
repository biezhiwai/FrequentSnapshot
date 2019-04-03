#!/usr/bin/env bash

for size in 250000 500000 1000000 2000000 
do
for alg in 0 1 2 3 4 5 6 
do
    ./run.sh $alg $size 4096 256 0.8
    done
done
