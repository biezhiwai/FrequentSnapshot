#!/usr/bin/env bash

for j in 16 32 64 128 256 
do
for i in 5 
do
    ./run.sh $i 250000 4096 $j 0.9
    done
done


for n in 250000 500000 1000000 2000000 
do
for m in 5 
do
    ./run.sh $m $n 4096 256 0.9
    done
done
