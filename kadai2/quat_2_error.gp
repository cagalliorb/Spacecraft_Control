set xlabel "time [s]"
set ylabel "q []"

set datafile separator ","
plot "quat2.csv" using 1:15 with lines title "q_2 error"
replot "quat2.csv" using 1:22 with lines title "+square of P(2,2)"
replot "quat2.csv" using 1:23 with lines title "-square of P(2,2)"
