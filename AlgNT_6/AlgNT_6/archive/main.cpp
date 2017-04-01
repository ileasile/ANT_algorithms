#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include "../../AlgNT_1/AlgNT_1/BigInt.h"

void eratosthenes(std::set<BigInt> & s, const BigInt & n) {
	s.clear();
	if (n < 2)	return;
	auto hint = s.insert(2).first;
	for (BigInt i = 3; i <= n; ++i) 
		hint = s.insert(hint, i);
	
	for (BigInt i = 2; i * i <= n; i = * s.upper_bound(i)) 
		for (BigInt j = i * i; j <= n; j += i) 
			s.erase(j);
}

double getCPUTime();
void test(BigInt nstart, const BigInt & nstep, int test_count, const std::string & fname) {
	std::ofstream f(fname);
	std::set<BigInt> s;
	for (int i = 1; i <= test_count; ++i, nstart+=nstep) {
		std::cout << i << "... ";
		auto time = getCPUTime();
		eratosthenes(s, nstart);
		time = getCPUTime() - time;
		f << i << ", " << nstart << ", " << time << "\n";
	}
	f.close();
}

int main() {
	std::set<BigInt> s;
	eratosthenes(s, 1000);
	for (auto & i : s) {
		std::cout << i << " ";
	}
	std::cout << std::endl;
	system("pause");
	

	//test(250000, 250000, 40, "test/eratosthenes.csv");
	return 0;
}