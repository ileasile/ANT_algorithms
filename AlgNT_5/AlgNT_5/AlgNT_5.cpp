#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include <NTL/ZZ.h>

using namespace NTL;
double getCPUTime();

void make_tests(std::function<double(int, ZZ &)> test_fun, int ntests, int step, int avg_by, std::string fname) {
	ZZ a;
	std::ofstream f(fname);

	for (int i = 1; i <= ntests; ++i) {
		std::cout << i << "... ";
		double time = 0;
		int len = i * step;

		for (int j = 1; j <= avg_by; ++j) {
			time += test_fun(len, a);
		}
		time /= avg_by;

		f << i << ", " << len << ", " << time << "\n";
	}

	f.close();
}

double test_prob(int len, ZZ & a) {
	RandomLen(a, len);
	double t = getCPUTime();
	bool b = ProbPrime(a);
	return getCPUTime() - t;
}

double test_generation(int len, ZZ & a) {
	double t = getCPUTime();
	GenPrime(a, len);
	return getCPUTime() - t;
}

int main(){
	ZZ a;
	const int NTESTS_PROB = 100;
	const int NTESTS_GEN = 50;
	const int STEP_PROB = 70;
	const int STEP_GEN = 20;
	const int AVGNUM_PROB = 50;
	const int AVGNUM_GEN = 50;

	make_tests(test_prob, NTESTS_PROB, STEP_PROB, AVGNUM_PROB, "test/prob.csv");
	make_tests(test_generation, NTESTS_GEN, STEP_GEN, AVGNUM_GEN, "test/gen.csv");

    return 0;
}

