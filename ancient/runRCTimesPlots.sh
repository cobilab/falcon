#!/bin/bash
gnuplot << EOF
  set terminal pdfcairo enhanced color
  set output 'time.pdf'
  set auto
  set boxwidth 0.45
  set xtics nomirror
  set style fill solid 1.00
  set ylabel 'Time (m)'
  set xlabel 'Methods'
  set grid ytics lc rgb '#C0C0C0'
  set style line 1 lc rgb "#3399FF"
  set style line 2 lc rgb "#008000"
  set style line 3 lc rgb "#CC0000"
  set style line 4 lc rgb "#6600CC"
  set style line 5 lc rgb "#CC00CC"
  unset key
  unset grid
  plot 'TIME' using 2:xtic(1) with boxes ls 1, \
  'TIME' using 2:xtic(1) with boxes ls 2, \
  'TIME' using 2:xtic(1) with boxes ls 3, \
  'TIME' using 2:xtic(1) with boxes ls 4, \
  'TIME' using 2:xtic(1) with boxes ls 5, \
  'TIME' using 2:xtic(1) with boxes ls 6
EOF

gnuplot << EOF
  set terminal pdfcairo enhanced color
  set output 'time2.pdf'
  set auto
  set boxwidth 0.45
  set xtics nomirror
  set style fill solid 1.00
  set ylabel 'Time (d)'
  set xlabel 'Methods'
  set grid ytics lc rgb '#C0C0C0'
  set style line 1 lc rgb "#3399FF"
  set style line 2 lc rgb "#008000"
  set style line 3 lc rgb "#CC0000"
  set style line 4 lc rgb "#6600CC"
  set style line 5 lc rgb "#CC00CC"
  unset key
  unset grid
  plot 'TIME2' using 2:xtic(1) with boxes ls 1, \
  'TIME2' using 2:xtic(1) with boxes ls 2, \
  'TIME2' using 2:xtic(1) with boxes ls 3, \
  'TIME2' using 2:xtic(1) with boxes ls 4, \
  'TIME2' using 2:xtic(1) with boxes ls 5
EOF

