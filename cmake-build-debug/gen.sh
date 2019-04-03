
for j in 250000 500000 1000000 2000000
do
	Rscript zipf.r 0.8 $j
	Rscript zipf.r 2.0 $j
done

for i in 0.2 0.4 0.6 
do
	Rscript zipf.r $i 1000000 
done

