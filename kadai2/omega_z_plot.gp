set xlabel "time [s]"
set ylabel "{/Symbol w} [rad/s]"

set datafile separator ","
plot "omega.csv" using 1:5 with lines title "{/Symbol w}_z true"
replot "omega.csv" using 1:9 with lines title "{/Symbol w}_z est"
