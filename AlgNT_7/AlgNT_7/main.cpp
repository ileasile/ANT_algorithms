#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>

#include <NTL/ZZ_pXFactoring.h>
#include <NTL/GF2XFactoring.h>

using namespace NTL;
using namespace std;

void zz_px_test() {
	cout << "===== ZZ_PX test =====\n";

	ZZ_p::init(ZZ(17));
	ZZ_pX p1, p2;
	string p1s("[2323 2323 12 30 33 0 2 11]");
	string p2s("[13 15 8 177 31]");
	stringstream io;

	//input
	io << p1s << " " << p2s;
	io >> p1 >> p2;

	//output
	std::cout << "P1 = " << p1 << "\nP2 = " << p2 << "\n";

	//multiplication
	auto mlt = p1 * p2;
	std::cout << "P1 * P2 = " << mlt << "\n";

	//division
	auto quo = p1 / p2;
	auto rem = p1 % p2;
	std::cout << "P1 / P2 = (" << quo << ", " << rem << ")\n";
	std::cout << "Division check: P2 * Q + R = " << p2 * quo + rem << " (should be " << p1 << ")\n";

	//Euclid algorithm
	ZZ_pX gcd;
	GCD(gcd, p1, p2);
	std::cout << "Euclid algorithm: (P1, P2) = " << gcd << "\n";

	//Extended Euclid algorithm
	ZZ_pX u, v;
	XGCD(gcd, u, v, p1, p2);
	std::cout << "Ext. Euclid algorithm: (P1, P2) = " << gcd << " = " << "P1 * " << u << " + P2 * " << v << "\n";

	//derivative of P1
	ZZ_pX dp1;
	diff(dp1, p1);
	std::cout << "d(P1)/dx = " << dp1 << "\n";

	//P1(a)
	ZZ_p a(2);
	auto b = eval(p1, a);
	std::cout << "P1(" << a << ") = " << b << "\n";

	//factorization of P1.
	//first make P1 monic
	auto ec = p1[deg(p1)];
	auto p1_monic = p1 / ec;
	//then factor monic polynomial
	Vec< Pair< ZZ_pX, long > > factors;
	CanZass(factors, p1_monic);
	cout << "Factorization of P1: " << ec << " * " << factors << "\n";

	//test for irreducibility
	cout << "Is P1 irreducible: " << DetIrredTest(p1) << "\n";
	cout << "Is P2 irreducible: " << DetIrredTest(p2) << "\n";

	//using ZZ_pXModulus. Using p1_monic as modulus
	ZZ_pXModulus F(p1_monic);
	auto res = ZZ_pX(1);
	int num_poly = 10;
	for (int i = 0; i < num_poly; ++i) {
		ZZ_pX p;
		BuildIrred(p, 6);
		MulMod(res, res, p, F);
	}
	cout << "Result of multiplication: " << res << "\n";

	//modular compozition
	ZZ_pX p;
	BuildIrred(p, 9);
	// x = g(h) mod f; deg(h) < n
	auto x = CompMod(p1, p2, p);
	cout << x << "\n";
	
}

void gf2x_test() {
	cout << "===== GF2X test =====\n";

	GF2X p1, p2;
	string p1s("[1 0 1 0 0 0 1 1]");
	string p2s("[1 0 0 0 1]");
	stringstream io;

	//input
	io << p1s << " " << p2s;
	io >> p1 >> p2;

	//output
	std::cout << "P1 = " << p1 << "\nP2 = " << p2 << "\n";

	//multiplication
	auto mlt = p1 * p2;
	std::cout << "P1 * P2 = " << mlt << "\n";

	//division
	auto quo = p1 / p2;
	auto rem = p1 % p2;
	std::cout << "P1 / P2 = (" << quo << ", " << rem << ")\n";
	std::cout << "Division check: P2 * Q + R = " << p2 * quo + rem << " (should be " << p1 << ")\n";

	//Euclid algorithm
	GF2X gcd;
	GCD(gcd, p1, p2);
	std::cout << "Euclid algorithm: (P1, P2) = " << gcd << "\n";

	//Extended Euclid algorithm
	GF2X u, v;
	XGCD(gcd, u, v, p1, p2);
	std::cout << "Ext. Euclid algorithm: (P1, P2) = " << gcd << " = " << "P1 * " << u << " + P2 * " << v << "\n";

	//derivative of P1
	GF2X dp1;
	diff(dp1, p1);
	std::cout << "d(P1)/dx = " << dp1 << "\n";
	
	//factorization of P1.
	//first make P1 monic
	auto ec = p1[deg(p1)];
	auto p1_monic = p1 / ec;
	//then factor monic polynomial
	Vec< Pair< GF2X, long > > factors;
	CanZass(factors, p1_monic);
	cout << "Factorization of P1: " << ec << " * " << factors << "\n";

	//test for irreducibility
	cout << "Is P1 irreducible: " << IterIrredTest(p1) << "\n";
	cout << "Is P2 irreducible: " << IterIrredTest(p2) << "\n";
	
	//modular compozition
	GF2X p;
	BuildIrred(p, 9);
	// x = g(h) mod f; deg(h) < n
	auto x = CompMod(p1, p2, p);
	cout << x << "\n";
}

int main() {
	zz_px_test();
	cout << "\n";
	gf2x_test();
	//system("pause");
	return 0;
}