#include "../../AlgNT_1/AlgNT_1/BigInt.h"
#include "Euclid.h"
#include <iostream>

int main() {
	BigInt c("1");
	BigInt a("0");
	BigInt b("0");
	a *= c;
	b *= c;
	std::cout << "a = " << a << ", b = " << b << std::endl;
	using namespace Euclid;

	std::cout << gcd(a, b, GCDCalcMethod::CLASSIC) << std::endl;
	std::cout << gcd(a, b, GCDCalcMethod::BINARY) << std::endl;
	std::cout << lcm(a, b, GCDCalcMethod::CLASSIC) << std::endl;
	std::cout << lcm(a, b, GCDCalcMethod::BINARY) << std::endl;
	
	BigInt u, v;
	BigInt g = gcd_ext(a, b, u, v, GCDCalcMethod::CLASSIC);
	std::cout << "CLASSIC. d = " << g << ", u = " << u << ", v = " << v << ", " << (g == u*a + b*v ? "OK" : "FAIL") << std::endl;
	g = gcd_ext(a, b, u, v, GCDCalcMethod::BINARY);
	std::cout << "BINARY. d = " << g << ", u = " << u << ", v = " << v << ", " << (g == u*a + b*v ? "OK" : "FAIL") << std::endl;

	system("pause");
	return 0;
}