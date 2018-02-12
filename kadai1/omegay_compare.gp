set xlabel "time [s]"
set ylabel "{/Symbol w} [rad/s]"

set datafile separator ","
plot "omega.csv" using 1:3 with lines title "{/Symbol w}_y(numerical answer)"
replot "omega_kaiseki.csv" using 6:17 with lines title "{/Symbol w}_y(analytical answer)"
