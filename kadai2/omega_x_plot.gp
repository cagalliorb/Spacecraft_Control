set xlabel "time [s]"
set ylabel "{/Symbol w} [rad/s]"

set datafile separator ","
plot "omega.csv" using 1:3 with lines title "{/Symbol w}_x true"
replot "omega.csv" using 1:7 with lines title "{/Symbol w}_x est"
