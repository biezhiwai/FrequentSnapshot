#!/usr/bin/env bash
#可更改
DB_SIZE=25000
UF=12800
THREAD_NUM=1
UNIT_SIZE=4096
#不可更改
ALG_NAME=("NAIVE" "COU" "ZIGZAG" "PINGPONG" "MK" "LL")
LOG_NAME=("naive" "cou" "zigzag" "pingpong" "mk" "ll")
RF_FILE=./rfg.txt
DATA_DIR=./log/overhead/
RESULT_DIR=./diagrams/experimental_result/
mkdir log
mkdir ckp_backup
mkdir log/overhead
mkdir log/latency
#1. generate the zipf random file
python Zipf.py $RF_FILE $UF $DB_SIZE

for i in 0 1 2 3 4 5 
do 
	echo ${ALG_NAME[i]}
	echo "-------------------------------------"
	ARG_CKP_LATENCY=${THREAD_NUM}" "${DB_SIZE}" "$i" "$RF_FILE" "${UF}" "$UNIT_SIZE
	#./bin/ckp_simulator $ARG_CKP_LATENCY
	echo "-------------------------------------"
done

PLOT_ARG=${DATA_DIR}" "${DB_SIZE}" "${UNIT_SIZE}" "${THREAD_NUM}" "${RESULT_DIR}
#python diagrams/tps_plot.py $PLOT_ARG
