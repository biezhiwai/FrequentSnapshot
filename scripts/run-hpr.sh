UF=32
PAGE_SIZE=64
DB_SIZE=16000000
alpha=1.7

for HPR in 0.00 0.30 0.50 0.70 0.90 0.95 0.99 1.00; do
    ./app 8 ${DB_SIZE} ${PAGE_SIZE} ${UF} ${alpha} ${HPR}
    python result-hpr.py ${UF} ${DB_SIZE} ${PAGE_SIZE} ${HPR} ${alpha} run-hpr
done

