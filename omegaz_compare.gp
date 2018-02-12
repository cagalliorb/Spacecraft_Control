
set xlabel "time [s]"
set ylabel "{/Symbol w} [rad/s]"

set datafile separator ","
plot "omega.csv" using 1:4 with lines title "{/Symbol w}_z(numerical answer)"
replot "omega_kaiseki.csv" using 6:18 with lines title "{/Symbol w}_z(analytical answer)"
