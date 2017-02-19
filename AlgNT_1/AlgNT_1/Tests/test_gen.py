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


f = open("tests.txt", "w");
BASE = 10
WRITE_RESULTS = True
#5 серий тестов
f.write("5\n");

#input/output
NTESTS = 20
f.write("io " + str(NTESTS) +"\n");
for i in range(NTESTS):
    f.write(rsnum(rand_int(i*20, (i+1)*20)));
    f.write("\n");


#unary arithmetics
NTESTSUNARY = 20;
f.write("ua " + str(NTESTSUNARY) +"\n");
for i in range(NTESTSUNARY):
    s = rsnum(rand_int(i*20, (i+1)*20))
    if WRITE_RESULTS:
        a = int(s, BASE);
        minus_a = -a;
        f.write(str(a) + " " + str(minus_a) + "\n");
    else:
        f.write(s + "\n");

#shifts
NTESTSSHIFT = 20;
f.write("shifts " + str(NTESTSSHIFT) +"\n");
for i in range(NTESTSSHIFT):
    s = rnum(rand_int(i*20, (i+1)*20))
    n = rand_int(0, 10000);
    if WRITE_RESULTS:
        a = int(s, BASE);
        res_left =  a<<n;
        res_right = a>>n;
        f.write(str(a) + " " + str(n) + " " + str(res_left) + " " + str(res_right) + "\n");
    else:
        f.write(s + " " + str(n) + "\n");

#comparison
NTESTS = 20;
f.write("comp " + str(NTESTS) +"\n");
for i in range(NTESTS):
    s1 = rsnum(rand_int(i*5, (i+1)*5))
    s2 = rsnum(rand_int(i*5, (i+1)*5))
    if WRITE_RESULTS:
        a = int(s1, BASE);
        b = int(s2, BASE);
        res = 0
        if a > b:
            res = 1
        if a < b:
            res = -1
        f.write(str(a) + " " + str(b) + " " + str(res) + "\n");
    else:
        f.write(s1 + " " + s2 + "\n");

#binary arithmetics
NTESTS = 20;
f.write("ba " + str(NTESTS) +"\n");
for i in range(NTESTS):
    s1 = rsnum(rand_int(i*50, (i+1)*50))
    s2 = rsnum(rand_int(max(1,i*50 - 100), i*50 +100))
    if WRITE_RESULTS:
        a = int(s1, BASE);
        b = int(s2, BASE);
        aplusb = a + b
        aminusb = a - b
        amultb = a*b
        adivb = a//b
        amodb = a%b
        f.write(str(a) + " " + str(b) + " " + str(aplusb) + " " + str(aminusb) + " " + str(amultb) + " " + str(adivb) + " " + str(amodb) +"\n");
    else:
        f.write(s1 + " " + s2 + "\n");

f.close();
