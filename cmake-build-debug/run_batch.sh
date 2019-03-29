#!/usr/bin/env bash

for i in 0 1 2 3 4 5 6
do
    for j in 16 32 64 128 256
    do
    ./run.sh 250000 $j 4096 $i
    done
done


for m in 0 1 2 3 4 5 6
do
    for n in 250000 500000 1000000 2000000
    do
    ./run.sh $n 256 4096 $m
    done
done
