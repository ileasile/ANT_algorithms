set term png

set output "eratosthenes.png"
plot 'eratosthenes.csv' using 2:3 with lines
