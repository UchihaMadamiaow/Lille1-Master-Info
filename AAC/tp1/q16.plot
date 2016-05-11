set term png;
set xrange [0:50];
set yrange[0:200];
set output 'q16.png';
plot 'q16.txt' using 1:2 title 'log' with lines, \
'q16.txt' using 1:3 title 'NpK' with lines, \
'q16.txt' using 1:4 title 'Nlogn' with lines, \
'q16.txt' using 1:5 title 'Exp' with lines, \
'q16.txt' using 1:6 title 'Kpn' with lines ;