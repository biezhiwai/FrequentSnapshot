set term pdfcairo lw 2 font "Arial,18"
set output "latency256k-part.pdf"

set xlabel "# Tick" font "Arial,24"
set ylabel "Latency[ms] "font "Arial,24"
#set logscale y
#set ytics ("2000" 2000,"4000" 4000,"6000" 6000,"8000" 8000,"10000" 10000, "......" 11000, "......" 12000, "10^6" 13000)
set key center top
set grid lw 2
set key maxcols 4
set key maxrows 1
set key samplen 3

plot [50:100][0:40] \
"../log/latency/2_latency_256k_250000_4096_0.log" using ($2/1000000) title "ZZ" with lines linecolor 9 linewidth 1.5 ,\
"../log/latency/4_latency_256k_250000_4096_0.log" using ($2/1000000) title "PB" with lines linecolor 11 linewidth 1.5 ,\
"../log/latency/5_latency_256k_250000_4096_0.log" using ($2/1000000) title "HG" with lines linecolor 12 linewidth 1.5 ,\
"../log/latency/6_latency_256k_250000_4096_0.log" using ($2/1000000) title "Fork" with lines linecolor 13 linewidth 1.5
