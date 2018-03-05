set xlabel "time [s]"
set ylabel "{/Symbol w} [rad/s]"

set datafile separator ","
plot "omega.csv" using 1:4 with lines title "{/Symbol w}_y true"
replot "omega.csv" using 1:8 with lines title "{/Symbol w}_y est"
