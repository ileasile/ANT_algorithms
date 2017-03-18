#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "../../AlgNT_3/AlgNT_3/Euclid.h"

void gcd(const std::vector<BigInt> & a, BigInt & d) {
	using namespace Euclid;

	if (a.size() == 1) {
		d = a[0];
		return;
	}

	d = gcd(a[0], a[1]);
	for (size_t i = 2; i < a.size(); ++i) {
		d = gcd(d, a[i]);
	}
}

void gcd_ext(const std::vector<BigInt> & a, std::vector<BigInt> & u, BigInt & d) {
	using namespace Euclid;

	int n = a.size();
	u.resize(n);
	if (n == 1) {
		u[0] = 1;
		d = a[0];
		return;
	}

	d = gcd_ext(a[0], a[1], u[0], u[1]);
	for (int i = 2; i < n; ++i) {
		BigInt _u;
		d = gcd_ext(d, a[i], _u, u[i]);
		for (int j = 0; j < i; ++j) {
			u[j] *= _u;
		}
	}
}

bool solveDi(const std::vector<BigInt> & a, const BigInt & b, std::vector<BigInt> & x) {
	BigInt d;
	gcd_ext(a, x, d);
	BigInt q, r;

	try {
		b.div(d, q, r);
	}
	catch (BigIntDivideByZeroException e) {
		return b.isNull();
	}

	// (a1, a2,..., an) | b - solution exists criteria
	if (!r.isNull()) {
		x.clear();
		return false;
	}

	for (auto & _x : x) {
		_x *= q;
	}
	return true;
}

int main() {
	int n;
	std::vector<BigInt> a;
	BigInt b;

	std::ifstream fin("input.txt");
	std::ofstream fout("output.txt");

	// change to std::cout / std::cin if necessary
	std::ostream & out = fout;
	std::istream & in = fin;
	
	//std::cout << "Enter the number of variables: ";
	in >> n;
	a.resize(n);
	for (int i = 1; i <= n; ++i) {
		//std::cout << "a_"<<i<<" = ";
		in >> a[i-1];
	}
	//std::cout << "b = ";
	in >> b;
	
	std::vector<BigInt> x;
	if (solveDi(a, b, x)) {
		BigInt res = 0;
		for (int i = 1; i <= n; ++i) {
			out << "x_" << i << " = " << x[i - 1] << "\n";
			res += a[i - 1] * x[i - 1];
		}
		out << ((res == b) ? "\n === Correct! === " : "\n === Incorrect! === ");
	}
	else {
		BigInt d;
		gcd(a, d);
		out << "This equation has no solutions.\n GCD is "<< d;
	}
	out << "\n\n";
	//system("pause");
	return 0;
}