THREAD_NUM='1'
ALG_ARRAY=(0 1 2 3 4 5)
ALG_NAME=("NAIVE" "COU" "ZIGZAG" "PINGPONG" "MK" "LL")
UNIT_SIZE=8192
DB_SIZE_ARRAY=("12500" "25000" "50000" "100000" "200000")
UF_ARRAY=("2" "4" "8" "16" "32")
UF_BASE="2"
RF_FILE="./rfg.txt"
LOG_DIR="./log/overhead/"
RESULT_DIR="./diagrams/data/"
EXP_DIR="./diagrams/experimental_result/"

rm ./diagrams/data/*
for eachAlg in 0 1 2 3 4 5
do
for eachSize in 0 1 2 3 4
do
	python ./overhead_format.py $eachAlg" "${DB_SIZE_ARRAY[eachSize]}" "${UF_ARRAY[eachSize]}" "$UNIT_SIZE" "$RESULT_DIR" "$LOG_DIR
done
done
python ./overhead_savePDF.py $RESULT_DIR $EXP_DIR
