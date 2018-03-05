set xlabel "time [s]"
set ylabel "error of {/Symbol w} [rad/s]"

set datafile separator ","
plot "omega.csv" using 1:12 with lines title "{/Symbol w}_y error"
replot "omega.csv" using 1:17 with lines title "+square of P(5,5)"
replot "omega.csv" using 1:18 with lines title "-square of P(5,5)"
