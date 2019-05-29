set term pdfcairo lw 2 font "Arial,18"
set output "latency16k.pdf"

set xlabel "# Tick" font "Arial,24"
set ylabel "Latency[ms] "font "Arial,24"
#set ytics ("2000" 2000,"4000" 4000,"6000" 6000,"8000" 8000,"10000" 10000, "......" 11000, "......" 12000, "10^6" 13000)
set key center top
set grid lw 2
set key maxcols 3
set key maxrows 2
set key samplen 3

plot [][300:5000] \
"../log/0_latency_16k_16000000_256_0.log" using ($1) title "NS" with lines linecolor 7 linewidth 1.5 ,\
"../log/1_latency_16k_16000000_256_0.log" using ($1) title "COU" with lines linecolor 8 linewidth 1.5 ,\
"../log/2_latency_16k_16000000_256_0.log" using ($1) title "ZZ" with lines linecolor 9 linewidth 1.5 ,\
"../log/3_latency_16k_16000000_256_0.log" using ($1) title "PP" with lines linecolor 10 linewidth 1.5 ,\
"../log/4_latency_16k_16000000_256_0.log" using ($1) title "PB" with lines linecolor 11 linewidth 1.5 ,\
"../log/5_latency_16k_16000000_256_0.log" using ($1) title "HG" with lines linecolor 12 linewidth 1.5 ,\
"../log/6_latency_16k_16000000_256_0.log" using ($1) title "Fork" with lines linecolor 13 linewidth 1.5
