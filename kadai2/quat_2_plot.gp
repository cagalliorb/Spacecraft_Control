set xlabel "time [s]"
set ylabel "q []"

set datafile separator ","
plot "quat.csv" using 1:5 with lines title "q_2 true"
replot "quat.csv" using 1:10 with lines title "q_2 est"
