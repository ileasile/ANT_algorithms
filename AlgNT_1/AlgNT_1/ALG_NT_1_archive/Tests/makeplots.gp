set term png
set output "graph_add.png"
plot "time_res_ba.csv" using 2:3 with lines
set output "graph_subs.png"
plot "time_res_ba.csv" using 2:4 with lines
set output "graph_mult.png"
plot "time_res_ba.csv" using 2:5 with lines
set output "graph_div.png"
plot "time_res_ba.csv" using 2:6 with lines
set output "graph_rem.png"
plot "time_res_ba.csv" using 2:7 with lines

set output "graph_shifts_left.png"
plot "time_res_shifts.csv" using 2:3 with lines
set output "graph_shifts_right.png"
plot "time_res_shifts.csv" using 2:4 with lines

set output "graph_unary.png"
plot "time_res_ua.csv" using 2:3 with lines

set output "graph_comparison.png"
plot "time_res_comp.csv" using 2:3 with lines

set output "graph_io.png"
plot "time_res_io.csv" using 2:3 with lines

