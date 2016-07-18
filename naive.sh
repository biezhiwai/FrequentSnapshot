THREAD_NUM='1'
DB_SIZE=25600
UF='10'
UNIT_SIZE=8192

RF_FILE="./rfg.txt"
mkdir log
mkdir ckp_backup
mkdir log/overhead
mkdir log/latency

ARG_CKP_LATENCY=${THREAD_NUM}" "${DB_SIZE}" 2 "$RF_FILE" "${UF}" "$UNIT_SIZE
./bin/ckp_simulator $ARG_CKP_LATENCY

