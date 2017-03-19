set term png

set output "all_test.png"
plot 'all_test.csv' using 2:3 with lines

set output "particular_test.png"
plot 'particular_test.csv' using 2:3 with lines

