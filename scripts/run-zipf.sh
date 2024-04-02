UF=32
HPR=0.90
PAGE_SIZE=64
DB_SIZE=16000000

for alpha in 1.1 1.3 1.5 1.7 1.9; do
# for alg in 0 1 2 3 4 8; do
    for alg in 2 8; do
        ./app ${alg} ${DB_SIZE}  ${PAGE_SIZE} ${UF} ${alpha} ${HPR}
    done
    python result.py ${UF} ${DB_SIZE} ${PAGE_SIZE} ${HPR} ${alpha} run-zipf
done