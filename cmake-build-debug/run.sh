#!/usr/bin/env bash
make
mkdir log
DB_SIZE=$1
PAGE_SIZE=$2
UF=$3
FILE=$4
for alg in 0 1 2 3 4 5 6
do
numactl --cpubind=0 --membind=0 ./app ${alg} ${DB_SIZE}  ${PAGE_SIZE} ${UF} ${FILE}
done
echo ==========
./result.sh  ${DB_SIZE}  ${PAGE_SIZE} ${UF}
