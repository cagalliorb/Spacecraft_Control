set xlabel "time [s]"
set ylabel "q []"

set datafile separator ","
plot "quat.csv" using 1:16 with lines title "q_3 error"
replot "quat.csv" using 1:24 with lines title "+square of P(3,3)"
replot "quat.csv" using 1:25 with lines title "-square of P(3,3)"
