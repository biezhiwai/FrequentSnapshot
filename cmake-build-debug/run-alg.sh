#!/usr/bin/env bash
make
mkdir log
alg=$1
DB_SIZE=$2
PAGE_SIZE=$3
UF=$4
FILE=$5
perf stat -e cache-misses numactl --cpubind=0 --membind=0 ./app ${alg} ${DB_SIZE}  ${PAGE_SIZE} ${UF} ${FILE}
python result.py $alg $UF $DB_SIZE $PAGE_SIZE
