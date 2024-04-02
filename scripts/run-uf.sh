HPR=0.90
PAGE_SIZE=64
DB_SIZE=16000000
alpha=1.7

for UF in 16 32 64 128 256; do
# for alg in 0 1 2 3 4 8; do
    for alg in 2 8; do
        ./app ${alg} ${DB_SIZE}  ${PAGE_SIZE} ${UF} ${alpha} ${HPR}
    done
    python result.py ${UF} ${DB_SIZE} ${PAGE_SIZE} ${HPR} ${alpha} run-uf
done
