set term png

set output "generate_prime.png"
plot 'gen.csv' using 2:3 with lines

set output "prob_prime.png"
plot 'prob.csv' using 2:3 with lines

