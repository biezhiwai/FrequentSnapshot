uf=$3
size=$1
page=$2
for i in 0 1 2 3 4 5 6
do
python result.py $i $uf $size $page
done
