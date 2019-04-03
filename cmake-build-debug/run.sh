#!/usr/bin/env bash
ALG=$1
DB_SIZE=$2
PAGE_SIZE=$3
UF=$4
ZIPF=$5
numactl --cpubind=0 --membind=0 ./app ${ALG} ${DB_SIZE}  ${PAGE_SIZE} ${UF} rfg-${ZIPF}-${DB_SIZE}.txt
