set term png
set out 'thresholdlatenciesstimuli.png'

set multiplot layout 4, 2

set border 3
unset key
set offset 0, 0, 0.5, 0.5
set xrange [-4:6]
set xlabel 'Time [a.u.]' 0, 0.5
set xtics 1 nomirror
set yrange [0:2]
set ytics 1 nomirror
set ylabel 'Current' 1, 0
set lmarg 5
set rmarg 1
set tmarg 1
set bmarg 3
set label 1 at graph 0.05, graph 1
set arrow 2 from -4, 0 to 6, 0 nohead lt 0
set arrow 3 from -4, 0.7 to 6, 0.7 nohead lt 0

set label 1 'A'
set label 3 'Test-pulse' at 0, 2
plot '-' w l lt 1 lw 4
-4 0
0 0
0 1
1 1
1 0
6 0
e

set label 1 'B'
plot '-' w l lt 1 lw 4
-4 0.7
0 0.7
0 1.2
1 1.2
1 0.7
6 0.7
e

set label 1 'C'
#set arrow 1 from 2, 0.5 to 4, 0.5 heads lt -1
set label 3 'Test-pulse' at 0, 2.5
set label 4 'Post-pulse' at 1.5, 2
plot '-' w l lt 1 lw 4
-4 0
0 0
0 1
1 1
1 0.7
3 0.7
3 0
6 0
e

set label 1 'D'
#set arrow 1 from 2, 0.5 to 4, 0.5 heads lt -1
plot '-' w l lt 1 lw 4
-4 0.7
0 0.7
0 1.2
1 1.2
1 0
3 0
3 0.7
6 0.7
e

set label 1 'E'
set label 2 'Pre-pulse' at -2, 2
set label 3 'Test-pulse' at 0, 2.5
unset label 4
#set arrow 1 from -3, 0.5 to -1, 0.5 heads lt -1
plot '-' w l lt 1 lw 4
-4 0
-2 0
-2 0.7
0 0.7
0 1
1 1
1 0
6 0
e

set label 1 'F'
#set arrow 1 from -3, 0.5 to -1, 0.5 heads lt -1
plot '-' w l lt 1 lw 4
-4 0.7
-2 0.7
-2 0
0 0
0 0.7
0 1.2
1 1.2
1 0.7
6 0.7
e

set label 1 'G'
#set arrow 1 from -3, 0.5 to -1, 0.5 heads lt -1
set label 2 'Pre-pulse' at -2, 2
set label 3 'Test-pulse' at 0, 2.5
set label 4 'Post-pulse' at 1.5, 2
plot '-' w l lt 1 lw 4
-4 0
-2 0
-2 0.7
0 0.7
0 1
1 1
1 0.7
4 0.7
4 0
6 0
e

set label 1 'H'
#set arrow 1 from -3, 0.5 to -1, 0.5 heads lt -1
plot '-' w l lt 1 lw 4
-4 0.7
-2 0.7
-2 0
0 0
0 0.7
0 1.2
1 1.2
1 0
4 0
4 0.7
6 0.7
e

unset multiplot

reset
