#!/usr/bin/env bash
#可更改
DB_SIZE=$1
UF=$2
PAGE_SIZE=$3
THREAD_NUM='1'

#不可更改
ALG_NAME=("NAIVE" "COU" "ZIGZAG" "PINGPONG" "MK" "LL" "MYFORK")
LOG_NAME=("naive" "cou" "zigzag" "pingpong" "mk" "ll" "myfork")
RF_FILE="./rfg.txt"
DATA_DIR="./log/latency/"
RESULT_DIR="./diagrams/experimental_result/"

# python ./Zipf.py $RF_FILE $UF $DB_SIZE

echo ${ALG_NAME[$4]}
echo "-------------------------------------"
./app ${THREAD_NUM} ${DB_SIZE} $4 $RF_FILE ${UF} $PAGE_SIZE
echo "-------------------------------------"


#PLOT_ARG=$UNIT_SIZE" "$DB_SIZE" "$UF" 0 "$DATA_DIR" "$RESULT_DIR

#python -i ./latency_plot.py $PLOT_ARG
