#include "../../AlgNT_1/AlgNT_1/BigInt.h"
#include "Euclid.h"
#include <iostream>
#include <fstream>

double getCPUTime();
class Timer {
	double st;
public:
	void start() {
		st = getCPUTime();
	}
	double get() {
		auto fn = getCPUTime();
		return (fn - st);
	}
	std::string getstr() {
		auto fn = getCPUTime();
		return std::to_string(fn - st);
	}
};

int main() {
	/*
	BigInt a("225992178819759345");
	BigInt b("1139333695783269");
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
	*/

	using namespace Euclid;

	std::ifstream f("tests/tests.txt");
	std::ofstream gc("tests/gcd_classic.csv");
	std::ofstream gb("tests/gcd_binary.csv");
	std::ofstream lc("tests/lcm_classic.csv");
	std::ofstream lb("tests/lcm_binary.csv");
	std::ofstream gce("tests/gcd_classic_extended.csv");
	std::ofstream gbe("tests/gcd_binary_extended.csv");

	BigInt a, b, g, l, u, v, gt, lt;
	Timer tm;
	std::string strtime;
	int N;
	bool OK, OKuv;

	f >> N;
	for (int i = 1; i <= N; ++i) {
		std::cout << i << "... ";
		f >> a >> b >> gt >> lt;
		auto sz = std::min(a.dig(), b.dig());
		std::string pre_info = std::to_string(i) + "; " + std::to_string(sz) + "; ";
		
		tm.start();
		g = gcd(a, b, GCDCalcMethod::CLASSIC);
		strtime = tm.getstr();
		OK = g == gt;
		gc << pre_info << strtime << "; " << (OK ? "OK" : "FAIL") << "\n";

		tm.start();
		g = gcd(a, b, GCDCalcMethod::BINARY);
		strtime = tm.getstr();
		OK = g == gt;
		gb << pre_info << strtime << "; " << (OK ? "OK" : "FAIL") << "\n";

		tm.start();
		l = lcm(a, b, GCDCalcMethod::CLASSIC);
		strtime = tm.getstr();
		OK = l == lt;
		lc << pre_info << strtime << "; " << (OK ? "OK" : "FAIL") << "\n";

		tm.start();
		l = lcm(a, b, GCDCalcMethod::BINARY);
		strtime = tm.getstr();
		OK = l == lt;
		lb << pre_info << strtime << "; " << (OK ? "OK" : "FAIL") << "\n";

		tm.start();
		g = gcd_ext(a, b, u, v, GCDCalcMethod::CLASSIC);
		strtime = tm.getstr();
		OK = g == gt;
		OKuv = g == a*u + b*v;
		gce << pre_info << strtime << "; " << (OK ? "OK" : "FAIL") << "; " << (OKuv ? "OK" : "FAIL") <<"\n";

		// because of stack overflow
		if (i <= 300) {
			tm.start();
			g = gcd_ext(a, b, u, v, GCDCalcMethod::BINARY);
			strtime = tm.getstr();
			OK = g == gt;
			OKuv = g == a*u + b*v;
			gbe << pre_info << strtime << "; " << (OK ? "OK" : "FAIL") << "; " << (OKuv ? "OK" : "FAIL") << "\n";
		}
	}

	gc.close();
	gb.close();
	lc.close();
	lb.close();
	gce.close();
	gbe.close();
	f.close();
	return 0;
}