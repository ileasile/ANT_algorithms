#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <functional>

#include "../../AlgNT_3/AlgNT_3/Euclid.h"
#include "SquareMatrix.h"

SquareMatrix<Euclid::BI> build_completion(const std::vector<Euclid::BI> & a) {
	using namespace Euclid;
	
	auto n = a.size();
	SquareMatrix<Euclid::BI> A(n);
	A[0][0] = a[0];
	if (n == 1) {
		return A;
	}

	Euclid::BI d, d1, u, v;
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
			A[i][k_1] = Euclid::BI(0);
		}
		for (size_t i = 0; i < k_1; ++i) {
			A[k_1][i] = (a[i] / d1) * v;
		}

		d1 = d;
	}

	return A;
}

void gcd(const std::vector<Euclid::BI> & a, Euclid::BI & d) {
	using namespace Euclid;

	if (a.size() == 0) {
		d = Euclid::BI(0);
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

void gcd_ext(const std::vector<Euclid::BI> & a, std::vector<Euclid::BI> & u, Euclid::BI & d) {
	using namespace Euclid;

	auto n = a.size();
	u.resize(n);

	if (n == 0) {
		d = Euclid::BI(0);
		return;
	}

	if (n == 1) {
		u[0] = 1;
		d = a[0];
		return;
	}

	d = gcd_ext(a[0], a[1], u[0], u[1]);
	for (size_t i = 2; i < n; ++i) {
		Euclid::BI _u;
		d = gcd_ext(d, a[i], _u, u[i]);
		for (size_t j = 0; j < i; ++j) {
			u[j] *= _u;
		}
	}
}

bool solve_Diophantine_eq(const std::vector<Euclid::BI> & a, const Euclid::BI & b, std::vector<Euclid::BI> & x) {
	Euclid::BI d;
	gcd_ext(a, x, d);
	Euclid::BI q, r;

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
	const std::vector<Euclid::BI> & a, 
	const Euclid::BI & b, 
	std::vector<Euclid::BI> & x,
	std::vector<std::vector<Euclid::BI>> & coeffs) 
{
	Euclid::BI d;
	gcd(a, d);
	if (d.isNull()) {
		if (b.isNull()) {
			return AllSolsResult::ANY_SOLUTION;
		}
		else {
			return AllSolsResult::NO_SOLUTIONS;
		}
	}

	Euclid::BI _b, r;
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

	auto A = build_completion(_a).specific_inverse();

	coeffs.resize(a.size());
	for (size_t i = 0; i < a.size(); ++i) {
		coeffs[i].resize(a.size() - 1);
		for (size_t j = 0; j < coeffs[i].size(); ++j) {
			coeffs[i][j] = A[i][j + 1];
		}
	}

	return AllSolsResult::REGULAR_SOLUTION;
}

void input_task(std::vector<Euclid::BI> & a, Euclid::BI & b, std::istream & in = std::cin) {
	int n;
	in >> n;
	a.resize(n);
	for (int i = 1; i <= n; ++i) {
		in >> a[i - 1];
	}
	in >> b;
}
void print_eq(const std::vector<Euclid::BI> & a, const Euclid::BI & b, std::ostream & out, std::vector<size_t> * ind = nullptr) {
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
void task_all(std::vector<Euclid::BI> & a, Euclid::BI & b, std::ostream & out = std::cout, bool no_output = false) {
	auto n = a.size();
	if (!no_output) {
		out << "There is an equation of " << n << " variables:\n";
		print_eq(a, b, out);
	}

	// Let's delete zero-valued coeffs.
	std::vector<size_t> ind;
	std::set<size_t> zero_valued_ind;

	int del = 0;
	for (size_t i = 0; i < n; ++i) {
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
	std::vector<Euclid::BI> x;
	std::vector<std::vector<Euclid::BI>> cf;
	Euclid::BI res, d;
	bool allSolsOK = true;
	auto  flag = find_all_solutions(a, b, x, cf);
	if (no_output)
		return;
	out << " === Solution ===\n";
	switch (flag)
	{
	case AllSolsResult::NO_SOLUTIONS:
		out << "This equation has no solutions.";
		break;
	case AllSolsResult::ANY_SOLUTION:
		out << "Any integers may be a solution of this equation.";
		break;
	case AllSolsResult::REGULAR_SOLUTION:

		for (size_t i = 1; i <= n; ++i) {
			out << "x_" << ind[i-1] << " = " << x[i - 1];
			for (size_t j = 1; j <= cf[i - 1].size(); ++j) {
				auto cur_cf = cf[i - 1][j - 1];
				if (cur_cf.isNull())
					continue;
				if (cur_cf.isNeg()) {
					cur_cf.negate();
					out << " - ";
				}
				else {
					out << " + ";
				}
				if (cur_cf != Euclid::BI(1))
					out << cur_cf << " ";
				out << "k_" << j;
			}
			out << "\n";
			res += a[i - 1] * x[i - 1];
		}

		//check coefficients
		for (size_t j = 0; j < n - 1; ++j) {
			Euclid::BI sum;
			for (size_t i = 0; i < n; ++i) {
				sum += cf[i][j] * a[i];
			}
			allSolsOK = allSolsOK && sum == Euclid::BI(0);
		}

		for (auto i : zero_valued_ind) {
			out << "x_" << i << " = k_" << n++<<"\n";
		}

		out << "(k_i are any integers)\n";

		out << ((res == b && allSolsOK) ? "\n === Correct! === " : "\n === Incorrect! === ");
		break;
	}
}
void task_part(std::vector<Euclid::BI> & a, Euclid::BI & b, std::ostream & out = std::cout, bool no_output = false) {
	auto n = a.size();
	if (!no_output) {
		out << "There is an equation of " << n << " variables:\n";
		print_eq(a, b, out);
	}

	//Solve and print the result
	std::vector<Euclid::BI> x;
	Euclid::BI res, d;

	auto  flag = solve_Diophantine_eq(a, b, x);
	if (no_output)
		return;

	out << " === Partial solution ===\n";
	if (!flag){
		out << "This equation has no solutions.";
	}
	else {
		for (size_t i = 1; i <= n; ++i) {
			out << "x_" << i << " = " << x[i - 1] << "\n";
			res += a[i - 1] * x[i - 1];
		}

		out << "(k_i are any integers)\n";
		out << (res == b? "\n === Correct! === " : "\n === Incorrect! === ");
	}
}

void generate_rand_vector(size_t size, size_t len, std::vector<Euclid::BI> & a, size_t delta_len = 5) {
	a.resize(size);
	for (size_t i = 0; i < size; ++i) {
		a[i] = Euclid::BI::get_random(unsigned(len + rand() % delta_len));
	}
}

double getCPUTime();

void make_task(
	std::function<void(std::vector<Euclid::BI> &, Euclid::BI &, std::ostream &, bool)> task, 
	bool no_output, size_t number_of_tests, size_t len_coef, size_t delta,
	std::string test_result_file, std::string output_file_prefix) {

	double tim;
	std::vector<Euclid::BI> a;
	Euclid::BI b;

	if (number_of_tests != 0) {
		std::ofstream test_res(test_result_file);
		std::ofstream f;
		for (size_t i = 1; i <= number_of_tests; ++i) {
			std::cout << i << "... ";
			auto thislen = i * len_coef;

			generate_rand_vector(i, thislen, a, delta);
			b = Euclid::BI::get_random(unsigned(thislen));
			if (!no_output)
				f.open("output/"+ output_file_prefix + std::to_string(i) + ".txt");
			tim = getCPUTime();
			task(a, b, f, no_output);
			tim = getCPUTime() - tim;
			test_res << i << "; " << i*thislen << "; " << tim << "\n";
			if (!no_output)
				f.close();
		}
		test_res.close();
	}
}



int main() {
	srand((unsigned)time(NULL));
	const bool NO_OUTPUT = false;
	const size_t NTESTS_ALL = 50;
	const size_t NTESTS_PART = 100;
	const size_t LEN_COEF_ALL = 2;
	const size_t LEN_COEF_PART = 5;
	const size_t DELTA_ALL = 5;
	const size_t DELTA_PART = 5;
	
	make_task(task_all, NO_OUTPUT, NTESTS_ALL, LEN_COEF_ALL, DELTA_ALL, "test/all_test.csv", "output_all_");
	make_task(task_part, NO_OUTPUT, NTESTS_PART, LEN_COEF_PART, DELTA_PART, "test/particular_test.csv", "output_particular_");	

	/*
	std::vector<Euclid::BI> a;
	Euclid::BI b;
	std::ifstream fin("input.txt");
	std::ofstream fout("output.txt");

	// change to std::cout / std::cin if necessary
	std::vector<Euclid::BI> a;
	Euclid::BI b;
	input_task(a, b, fin);
	task_all(a, b, fout);
	
	fin.close();
	fout.close();*/
	
	return 0;
}