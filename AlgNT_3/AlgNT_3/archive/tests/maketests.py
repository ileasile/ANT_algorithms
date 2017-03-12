import fractions
import random

random.seed();
numerals = "0123456789abcdefghijklmnopqrstuvwxyz";


def strdig(dig, bigCase = True):
    if bigCase:
        return numerals[dig].upper();
    else:
        return numerals[dig];
def rand_digit(base):
    return min(int(random.uniform(0, base)), base -1);
def rand_nonzero_digit(base):
    return min(int(random.uniform(1, base)), base -1);
def rand_int(a, b):
    return int(random.uniform(a, b + 1));

def rand_num(n, base):
    if n == 0:
        return "0";
    n = n-1
    res = ""+strdig(rand_nonzero_digit(base));
    for i in range(n):
        res += strdig(rand_digit(base));
    return res;

def rnum(n):
    return rand_num(n, BASE);

def rsnum(n):
    return ("" if rand_int(0,1) == 0 else "-") + rnum(n)

def gcd_ext(a, b):
	v = u0 = 0;
	u = v0 = 1;
	

	while (b != 0) :
		q, r = divmod(a, b);
		a = b;
		b = r;

		ut = u0;
		u0 = u - u0*q;
		u = ut;

		vt = v0;
		v0 = v - v0*q;
		v = vt;
	return (a, u, v);

f = open("tests.txt", "w");
BASE = 10

NTESTS = 300;
f.write(str(NTESTS) +"\n");
for i in range(NTESTS):
    s1 = rnum(rand_int(i*40, (i+1)*40))
    s2 = rnum(rand_int(i*40, (i+1)*40))
    s3 = rnum(rand_int(i*40, (i+1)*40))
    c = int(s3, BASE);
    a = c * int(s1, BASE);
    b = c * int(s2, BASE);
    #g, u, v = gcd_ext(a, b);
    g = fractions.gcd(a, b);
    l = (a*b)//fractions.gcd(a,b);
    f.write(str(a) + " " + str(b) + " " + str(g) + " " + str(l) + "\n");


f.close();