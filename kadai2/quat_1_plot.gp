set xlabel "time [s]"
set ylabel "q []"

set datafile separator ","
plot "quat.csv" using 1:4 with lines title "q_1 true"
replot "quat.csv" using 1:9 with lines title "q_1 est"
