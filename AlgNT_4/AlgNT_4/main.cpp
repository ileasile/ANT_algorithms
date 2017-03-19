#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>
#include <cstdlib>

#include "../../AlgNT_3/AlgNT_3/Euclid.h"
#include "SquareMatrix.h"

SquareMatrix<BigInt> build_completion(const std::vector<BigInt> & a) {
	using namespace Euclid;
	
	auto n = a.size();
	SquareMatrix<BigInt> A(n);
	A[0][0] = a[0];
	if (n == 1) {
		return A;
	}

	BigInt d, d1, u, v;
	d1 = gcd_ext(a[0], a[1], u, v);
	A[0][1] = a[1];
	A[1][1] = u;
	A[1][0] = v.negate();

	for (size_t k = 3; k <= n; ++k) {
		auto k_1 = k - 1;
		d = gcd_ext(d1, a[k_1], u, v);
		v.negate();

		A[0][k_1] = a[k_1];
		A[k_1][k_1] = u;
		for (size_t i = 1; i < k_1; ++i) {
			A[i][k_1] = BigInt(0);
		}
		for (size_t i = 0; i < k_1; ++i) {
			A[k_1][i] = (a[i] / d1) * v;
		}

		d = d1;
	}

	return A;
}

void gcd(const std::vector<BigInt> & a, BigInt & d) {
	using namespace Euclid;

	if (a.size() == 0) {
		d = BigInt(0);
		return;
	}

	if (a.size() == 1) {
		d = a[0];
		return;
	}

	d = gcd(a[0], a[1], GCDCalcMethod::BINARY);
	for (size_t i = 2; i < a.size(); ++i) {
		d = gcd(d, a[i], GCDCalcMethod::BINARY);
	}
}

void gcd_ext(const std::vector<BigInt> & a, std::vector<BigInt> & u, BigInt & d) {
	using namespace Euclid;

	int n = a.size();
	u.resize(n);

	if (n == 0) {
		d = BigInt(0);
		return;
	}

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

bool solve_Diophantine_eq(const std::vector<BigInt> & a, const BigInt & b, std::vector<BigInt> & x) {
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


enum class AllSolsResult{
	NO_SOLUTIONS, ANY_SOLUTION, REGULAR_SOLUTION
};

//may be called only if a[i] != 0 for any i OR if a[i] = 0 for any i.
AllSolsResult find_all_solutions(
	const std::vector<BigInt> & a, 
	const BigInt & b, 
	std::vector<BigInt> & x,
	std::vector<std::vector<BigInt>> & coeffs) 
{
	BigInt d;
	gcd(a, d);
	if (d.isNull()) {
		if (b.isNull()) {
			return AllSolsResult::ANY_SOLUTION;
		}
		else {
			return AllSolsResult::NO_SOLUTIONS;
		}
	}

	BigInt _b, r;
	b.div(d, _b, r);
	if (!r.isNull()) {
		return AllSolsResult::NO_SOLUTIONS;
	}

	auto _a = a;
	for (auto & el : _a) {
		el /= d;
	}

	solve_Diophantine_eq(_a, 1, x);
	for (auto & el : x) {
		el *= _b;
	}

	auto A = build_completion(_a).inverse();

	coeffs.resize(a.size());
	for (size_t i = 0; i < a.size(); ++i) {
		coeffs[i].resize(a.size() - 1);
		for (size_t j = 0; j < coeffs[i].size(); ++j) {
			coeffs[i][j] = A[i][j + 1];
		}
	}

	return AllSolsResult::REGULAR_SOLUTION;
}

void input_task(std::vector<BigInt> & a, BigInt & b, std::istream & in = std::cin) {
	int n;
	in >> n;
	a.resize(n);
	for (int i = 1; i <= n; ++i) {
		in >> a[i - 1];
	}
	in >> b;
}
void print_eq(const std::vector<BigInt> & a, const BigInt & b, std::ostream & out, std::vector<size_t> * ind = nullptr) {
	if (a.size() == 0)
		out << "0 ";

	else
		for (size_t i = 1; i <= a.size(); ++i) {
			if (a[i - 1].isNeg()) {
				out << "- " << -a[i - 1];
			}
			else {
				if (i != 1)
					out << "+ ";
				out << a[i - 1];
			}
			out << " x_" << (ind != nullptr ? (*ind)[i-1] : i) << " ";
		}

	out << "= " << b << "\n\n";
}
void task(std::vector<BigInt> & a, BigInt & b, std::ostream & out = std::cout) {
	int n = a.size();
	out << "There is an equation of " << n << " variables:\n";
	print_eq(a, b, out);

	// Let's delete zero-valued coeffs.
	std::vector<size_t> ind;
	std::set<size_t> zero_valued_ind;

	int del = 0;
	for (int i = 0; i < n; ++i) {
		if (a[i].isNull()) {
			zero_valued_ind.insert(i + 1);
			++del;
			continue;
		}
		a[i - del] = a[i];
		ind.push_back(i + 1);
	}
	a.resize(n -= del);

	//Uncomment if you want to see reduced equation
	/*
	if (del != 0) {
		out << "There is a reduced equation of "<< n <<" variables. \n"<<
			"We'll solve it. \n";
		print_eq(a, b, out, &ind);
	}
	*/

	//Solve and print the result
	std::vector<BigInt> x;
	std::vector<std::vector<BigInt>> cf;
	BigInt res, d;

	out << " === Solution ===\n";
	switch (find_all_solutions(a, b, x, cf))
	{
	case AllSolsResult::NO_SOLUTIONS:
		out << "This equation has no solutions.";
		break;
	case AllSolsResult::ANY_SOLUTION:
		out << "Any integers may be a solution of this equation.";
		break;
	case AllSolsResult::REGULAR_SOLUTION:

		for (int i = 1; i <= n; ++i) {
			out << "x_" << ind[i-1] << " = " << x[i - 1];
			for (size_t j = 1; j <= cf[i - 1].size(); ++j) {
				if (cf[i - 1][j - 1].isNeg()) {
					out << " - " << -cf[i - 1][j - 1];
				}
				else {
					out << " + " << cf[i - 1][j - 1];
				}
				out << " k_" << j;
			}
			out << "\n";
			res += a[i - 1] * x[i - 1];
		}
		
		for (auto i : zero_valued_ind) {
			out << "x_" << i << " = k_" << n++<<"\n";
		}

		out << "(k_i are any integers)\n";

		out << ((res == b) ? "\n === Correct! === " : "\n === Incorrect! === ");
		break;
	}
}

void generate_rand_vector(size_t size, size_t len, std::vector<BigInt> & a, size_t delta_len = 5) {
	a.resize(size);
	for (size_t i = 0; i < size; ++i) {
		a[i] = BigInt::get_random(len + rand() % delta_len);
	}
}

double getCPUTime();

int main() {
	srand((unsigned)time(NULL));
	std::vector<BigInt> a, x;
	BigInt b;
	const size_t NTESTS_ALL = 0;
	const size_t NTESTS_PART = 300;
	const size_t LEN_COEF_ALL = 1;
	const size_t LEN_COEF_PART = 5;
	double tim;

	std::ofstream test_allsols("test/all_test.csv");
	for (size_t i = 1; i <= NTESTS_ALL; ++i) {
		std::cout << i << "... ";
		auto thislen = i *LEN_COEF_ALL;

		generate_rand_vector(i, thislen, a, 1);
		b = BigInt::get_random(thislen);
		std::string fname = "output/output_" + std::to_string(i) + ".txt";
		std::ofstream f(fname);
		tim = getCPUTime();
		//task(a, b, f);
		tim = getCPUTime() - tim;
		test_allsols << i << "; " << thislen << "; " << tim << "\n";
		f.close();
	}
	test_allsols.close();

	std::ofstream test_partsol("test/particular_test.csv");
	for (size_t i = 1; i <= NTESTS_PART; ++i) {
		std::cout << i << "... ";
		auto thislen = i *LEN_COEF_PART;

		generate_rand_vector(i, thislen, a, 1);
		b = BigInt::get_random(thislen);
		tim = getCPUTime();
		solve_Diophantine_eq(a, b, x);
		tim = getCPUTime() - tim;
		test_partsol << i << "; " << thislen << "; " << tim << "\n";
	}
	test_partsol.close();
	

	/*
	std::ifstream fin("input.txt");
	std::ofstream fout("output.txt");

	// change to std::cout / std::cin if necessary
	std::vector<BigInt> a;
	BigInt b;
	input_task(a, b, fin);
	task(a, b, fout);
	
	fin.close();
	fout.close();
	*/
	return 0;
}