set xlabel "time [s]"
set ylabel "q []"

set datafile separator ","
plot "quaternion.csv" using 1:2 with lines title "q_0"
replot "quaternion.csv" using 1:3 with lines title "q_1"
replot "quaternion.csv" using 1:4 with lines title "q_2"
replot "quaternion.csv" using 1:5 with lines title "q_3"
