#!/usr/bin/env bash
make
mkdir log
DB_SIZE=$1
PAGE_SIZE=$2
UF=$3
for alg in 0 1 2 3 4 5 6
do
./app ${alg} ${DB_SIZE}  ${PAGE_SIZE} ${UF} $4
done
echo ==========
./result.sh  ${DB_SIZE}  ${PAGE_SIZE} ${UF}
