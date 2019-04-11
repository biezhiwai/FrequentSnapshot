#!/usr/bin/env bash

for size in 250000 500000 1000000 2000000 
do
    ./run.sh $size 4096 256
done
