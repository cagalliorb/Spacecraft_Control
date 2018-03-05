set xlabel "time [s]"
set ylabel "q []"

set datafile separator ","
plot "quat.csv" using 1:6 with lines title "q_3 true"
replot "quat.csv" using 1:11 with lines title "q_3 est"
