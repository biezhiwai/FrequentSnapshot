#!/usr/bin/env bash
DB_SIZE=$1
PAGE_SIZE=$2
UF=$3
ZIPF=$4
for alg in 0 1 2 3 4 5 6
do
numactl --cpubind=0 --membind=0 ./app ${alg} ${DB_SIZE}  ${PAGE_SIZE} ${UF} rfg-${ZIPF}-${DB_SIZE}.txt
done
