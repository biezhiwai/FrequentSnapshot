#!/usr/bin/env bash

for i in 0 1 2 3 4 5 6
do
    for j in 250000 500000 1000000 2000000
    do
    ./run.sh $j 256 4096 $i
    done
done




