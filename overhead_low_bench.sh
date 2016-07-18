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

mkdir log
mkdir log/overhead
mkdir log/latency
mkdir ckp_backup
eachAlg=$1

#       python ./src/zipf_create/uniform_create.py ${RF_FILE} ${UF_ARRAY[eachSize]} ${DB_SIZE_ARRAY[eachSize]}
for eachSize in 0 1 2 3 4
do
	echo ${ALG_NAME[eachAlg]} ${UF_ARRAY[eachSize]} ${DB_SIZE_ARRAY[eachSize]}
	echo "---------------------------------------------"
	ARG_CKP_SIMULATOR=${THREAD_NUM}" "${DB_SIZE_ARRAY[eachSize]}" "$eachAlg" "$RF_FILE" "${UF_ARRAY[eachSize]}" "$UNIT_SIZE
    ./bin/ckp_simulator $ARG_CKP_SIMULATOR
	echo "---------------------------------------------"
done

