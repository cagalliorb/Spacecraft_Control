set xlabel "time [s]"
set ylabel "q []"

set datafile separator ","
plot "quat2.csv" using 1:13 with lines title "q_0 error"
replot "quat2.csv" using 1:18 with lines title "+square of P(0,0)"
replot "quat2.csv" using 1:19 with lines title "-square of P(0,0)"
