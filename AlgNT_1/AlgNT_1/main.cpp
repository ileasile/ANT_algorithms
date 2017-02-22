#include "BigInt.h"
#include "Testing.h"

int main() {
	/*BigInt b(2123);
	BigInt rsum = (BigInt)123 + b;
	std::cout << rsum <<"\n";

	BigInt s(1048576);
	for (int i = 0; i < 65; ++i) {
		std::cout << s <<"\n";
		s <<= 1;
	}

	BigInt bigval("2563472368215925492456213426832565658789968274764");
	BigInt::outputBase = 10;
	std::cout << bigval << "\n";

	BigInt f1("187273809760761819675900464260275043294146641");
	BigInt f2("65580427813");
	BigInt f3(16*31*41);
	BigInt f4(4597);
	std::cout << f1*f2*f3*f4 <<"\n";

	BigInt a1(9);
	BigInt a2(4);

	BigInt bigval2("12341322344444444444444444444441341");
	std::cout << bigval / bigval2 << "\n";
	std::cout << bigval % bigval2 << "\n";
	std::cout << "\n" <<
		bigval *bigval2 << "\n" <<
		-bigval *bigval2 << "\n" <<
		bigval *-bigval2 << "\n" <<
		-bigval *-bigval2 << "\n" <<
		bigval * (BigInt)0 << "\n" <<
		(BigInt)0 * bigval2 << "\n" <<
		(BigInt)0 * 0 << "\n\n" <<

		bigval + bigval2 << "\n" <<
		bigval - bigval2 << "\n" <<
		-bigval + bigval2 << "\n" <<
		-bigval - bigval2 << "\n\n" <<
		bigval2 + bigval << "\n" <<
		bigval2 - bigval << "\n" <<
		-bigval2 + bigval << "\n" <<
		-bigval2 - bigval << "\n\n";

	std::cout << a1 / a2 << " " << a1 % a2 << "\n";
	std::cout << -a1 / a2 << " " << -a1 % a2 << "\n";
	std::cout << a1 / -a2 << " " << a1 % -a2 << "\n";
	std::cout << -a1 / -a2 << " " << -a1 % -a2 << "\n";
	system("pause");*/
	Testing::Table t;
	std::ofstream of;
	t = Testing::test_correctness("Tests/tests.txt", 20);
	of.open("Tests/correctness_res.csv");
	of << t;
	of.close();
	
	using Testing::TestType;
	auto ts = Testing::test_time("Tests/tests2.txt", {
		{ TestType::IO, 200 },
		{ TestType::UNARY, 1000 },
		{ TestType::BINARY, 1000 },
		{ TestType::SHIFTS, 1000 },
		{ TestType::COMP, 3000 } });
	for (auto p : ts) {
		of.open("Tests/time_res_"+p.first+".csv");
		of << p.second;
		of.close();
	}
	/*BigInt a("9999999999999999999999999999999999999999999999999999999999999999999999999999999");
	BigInt b("100000000000000000000000000000000000000000000000000");
	a <<= 64;
	int n = 0;
	std::cout << (a/b);
	system("pause");*/
	return 0;
}


