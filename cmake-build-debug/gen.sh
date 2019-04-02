
for j in 250000 500000 1000000 2000000
do
	Rscript zipf.r 0.8 $j
	Rscript zipf.r 0.9 $j
done

for i in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9
do
Rscript zipf.r $i 250000 
done

