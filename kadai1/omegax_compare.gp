set xlabel "time [s]"
set ylabel "{/Symbol w} [rad/s]"

set datafile separator ","
plot "omega.csv" using 1:2 with lines title "{/Symbol w}_x(numerical answer)"
replot "omega_kaiseki.csv" using 6:16 with lines title "{/Symbol w}_x(analytical answer)"

