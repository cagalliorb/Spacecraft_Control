set xlabel "time [s]"
set ylabel "{/Symbol w} [rad/s]"

set datafile separator ","
plot "omega.csv" using 1:2 with lines title "{/Symbol w}_x"
replot "omega.csv" using 1:3 with lines title "{/Symbol w}_y"
replot "omega.csv" using 1:4 with lines title "{/Symbol w}_z"

