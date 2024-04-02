UF=32
HPR=0.90
alpha=1.7

for PAGE_SIZE in 64 128 256 512 1024; do
    DB_SIZE=$[1024000000/PAGE_SIZE]
# for alg in 0 1 2 3 4 8; do
    for alg in 1 3; do
        ./app ${alg} ${DB_SIZE}  ${PAGE_SIZE} ${UF} ${alpha} ${HPR}
    done
    python result.py ${UF} ${DB_SIZE} ${PAGE_SIZE} ${HPR} ${alpha} run-row
done