set term png

set output "graph_add.png"
plot "time_res_add.csv" using 2:3 with lines

set output "graph_sub.png"
plot "time_res_sub.csv" using 2:3 with lines

set output "graph_mult.png"
plot "time_res_mult.csv" using 2:3 with lines

set output "graph_div.png"
plot "time_res_div.csv" using 2:3 with lines

set output "graph_comparison.png"
plot "time_res_comparison.csv" using 2:3 with lines

set output "graph_input.png"
plot "time_res_input.csv" using 2:3 with lines

set output "graph_output.png"
plot "time_res_output.csv" using 2:3 with lines

set output "graph_left_sh.png"
plot "time_res_left_sh.csv" using 2:3 with lines

set output "graph_right_sh.png"
plot "time_res_right_sh.csv" using 2:3 with lines

set output "graph_unary.png"
plot "time_res_unary.csv" using 2:3 with lines


