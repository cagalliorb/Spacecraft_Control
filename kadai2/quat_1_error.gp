set xlabel "time [s]"
set ylabel "q []"

set datafile separator ","
plot "quat2.csv" using 1:14 with lines title "q_1 error"
replot "quat2.csv" using 1:20 with lines title "+square of P(1,1)"
replot "quat2.csv" using 1:21 with lines title "-square of P(1,1)"
