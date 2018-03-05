set xlabel "time [s]"
set ylabel "error of {/Symbol w} [rad/s]"

set datafile separator ","
plot "omega.csv" using 1:13 with lines title "{/Symbol w}_z error"
replot "omega.csv" using 1:19 with lines title "+square of P(6,6)"
replot "omega.csv" using 1:20 with lines title "-square of P(6,6)"
