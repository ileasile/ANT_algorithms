#include "BigInt.h"

int main() {
	BigInt b(2123);
	BigInt rsum = (BigInt)123 + b;
	std::cout << rsum <<"\n";

	BigInt s(1048576);
	for (int i = 0; i < 65; ++i) {
		std::cout << s <<"\n";
		s <<= 1;
	}

	BigInt bigval("11481306952742545242328332011768769444444444444444444444444444444888888888888886757444444444444444444444444448669999999999999999999999999999999999999555555555555555557777777777777777777777777777777777777777777777777777999997681984022317702088695200477642736");
	BigInt::outputBase = 10;
	std::cout << bigval << "\n";

	BigInt f1("187273809760761819675900464260275043294146641");
	BigInt f2("65580427813");
	BigInt f3(16*31*41);
	BigInt f4(4597);
	std::cout << f1*f2*f3*f4 <<"\n";

	BigInt bigval2("12341322344444444444444444444441341");
	std::cout << bigval / bigval2 << "\n";
	std::cout << bigval % bigval2 << "\n";
	std::cout << "\n" <<
		bigval *bigval2 << "\n" << 
		-bigval *bigval2 << "\n" <<
		bigval *-bigval2 << "\n" <<
		-bigval *-bigval2 << "\n" <<
		bigval * (BigInt)0 << "\n" <<
		(BigInt)0 *bigval2 << "\n" <<
		(BigInt)0 * 0 << "\n\n" <<
		
		bigval + bigval2 << "\n" <<
		bigval - bigval2 << "\n" << 
		-bigval + bigval2 <<"\n" <<
		-bigval - bigval2 <<"\n\n" <<
		bigval2 + bigval << "\n" <<
		bigval2 - bigval << "\n" <<
		-bigval2 + bigval <<"\n" <<
		-bigval2 - bigval <<"\n";
	system("pause");
}


