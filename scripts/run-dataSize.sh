UF=32
HPR=0.90
PAGE_SIZE=64
alpha=1.7

for DB_SIZE in 1000000 2000000 4000000 8000000 16000000; do
# for alg in 0 1 2 3 4 8; do
    for alg in 8 2; do
        ./app ${alg} ${DB_SIZE}  ${PAGE_SIZE} ${UF} ${alpha} ${HPR}
    done
    python result.py ${UF} ${DB_SIZE} ${PAGE_SIZE} ${HPR} ${alpha} run-dataSize
done