set xlabel "time [s]"
set ylabel "q []"

set datafile separator ","
plot "quat.csv" using 1:3 with lines title "q_0 true"
replot "quat.csv" using 1:8 with lines title "q_0 est"

