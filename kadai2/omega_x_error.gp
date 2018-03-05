set xlabel "time [s]"
set ylabel "error of {/Symbol w} [rad/s]"

set datafile separator ","
plot "omega.csv" using 1:11 with lines title "{/Symbol w}_x error"
replot "omega.csv" using 1:15 with lines title "+square of P(4,4)"
replot "omega.csv" using 1:16 with lines title "-square of P(4,4)"
