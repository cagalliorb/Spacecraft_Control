
set xlabel "time [s]"
set ylabel "{/Symbol w} [rad/s]"

set datafile separator ","
plot "omega.csv" using 1:3 with lines title "{/Symbol w}_x true"
replot "omega.csv" using 1:4 with lines title "{/Symbol w}_y true"
replot "omega.csv" using 1:5 with lines title "{/Symbol w}_z true"
replot "omega.csv" using 1:7 with lines title "{/Symbol w}_x est"
replot "omega.csv" using 1:8 with lines title "{/Symbol w}_y est"
replot "omega.csv" using 1:9 with lines title "{/Symbol w}_z est"
