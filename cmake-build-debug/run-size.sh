#!/usr/bin/env bash

for n in 250000 500000 1000000 2000000 
do
for m in 0 1 2 3 4 5 6 
do
    ./run.sh $m $n 4096 256 0.9
    done
done
