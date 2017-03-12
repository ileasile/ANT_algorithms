set term png

set output "gcd.png"
plot 'gcd_classic.csv' using 2:3 with lines title "Classic", 'gcd_binary.csv' using 2:3 with lines title "Binary"

set output "lcm.png"
plot 'lcm_classic.csv' using 2:3 with lines title "Classic", 'lcm_binary.csv' using 2:3 with lines title "Binary"

set output "gcd_ext.png"
plot 'gcd_classic_extended.csv' using 2:3 with lines title "Classic", 'gcd_binary_extended.csv' using 2:3 with lines title "Binary"
