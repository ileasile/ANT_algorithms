#include <mpirxx.h>
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>

enum class CalcType {
	STRAIGHT, LAZY
};

class LasyRecursiveSeq
{
	typedef std::function<mpz_class(int, LasyRecursiveSeq &)> func_type;
	typedef std::vector<mpz_class> vec;
	static const mpz_class DEFAULT;

	func_type func;
	vec data;
	CalcType ct = CalcType::STRAIGHT;

	bool isset(int i) {
		return i < data.size() && data[i] != DEFAULT;
	}
	void resize(int i) {
		if (i >= data.size()) {
			data.resize(i+1, DEFAULT);
		}
	}

public:
	LasyRecursiveSeq(func_type func) :func(func) {}
	//may cause stack overflow
	mpz_class lazy(int i) {
		ct = CalcType::LAZY;
		if (isset(i)) {
			return data[i];
		}
		resize(i);
		return data[i] = func(i, *this);
	}
	//sholdn't cause stack overflow
	mpz_class straight(int i) {
		ct = CalcType::STRAIGHT;
		if (isset(i)) {
			return data[i];
		}
		resize(i);
		for (int j = 0; j <= i; ++j) {
			data[j] = func(j, *this);
		}
		return data[i];
	}
	CalcType calc_type() {
		return ct;
	}
	CalcType calc_type(CalcType nct) {
		auto oct = ct;
		ct = nct;
		return oct;
	}
	mpz_class operator[](int i) {
		switch (ct)
		{
		case CalcType::STRAIGHT:
			return straight(i);
		case CalcType::LAZY:
		default:
			return lazy(i);
		}
	}
	
};
const mpz_class LasyRecursiveSeq::DEFAULT = -1;

class LasyRecursiveSeq2
{
	typedef std::function<mpz_class(int, int, LasyRecursiveSeq2 &)> func_type;
	typedef std::vector<std::vector<mpz_class>> vec;
	static const mpz_class DEFAULT;

	func_type func;
	vec data;
	CalcType ct = CalcType::STRAIGHT;

	void resize(int i, int j) {
		if (i >= data.size()) {
			data.resize(i + 1);
		}
		for (int k = 0; k <= i; ++k) {
			if(j >= data[k].size())
				data[k].resize(j + 1, DEFAULT);
		}
	}
	bool isset(int i, int j) {
		return i < data.size() && j < data[i].size() && data[i][j] != DEFAULT;
	}
	
public:
	LasyRecursiveSeq2(func_type func) :func(func) {}
	//may cause stack overflow
	mpz_class lazy(int i, int j) {
		ct = CalcType::LAZY;
		if (isset(i, j)) {
			return data[i][j];
		}
		resize(i, j);
		return data[i][j] = func(i, j, *this);
	}
	//sholdn't cause stack overflow
	mpz_class straight(int i, int j) {
		ct = CalcType::STRAIGHT;
		if (isset(i, j)) {
			return data[i][j];
		}
		resize(i, j);
		for (int i0 = 0; i0 <= i; ++i0) {
			for (int j0 = 0; j0 <= j; ++j0) {
				data[i0][j0] = func(i0, j0, *this);
			}
		}

		return data[i][j];
	}
	CalcType calc_type() {
		return ct;
	}
	CalcType calc_type(CalcType nct) {
		auto oct = ct;
		ct = nct;
		return oct;
	}
	mpz_class at(int i, int j) {
		switch (ct)
		{
		case CalcType::STRAIGHT:
			return straight(i, j);
		case CalcType::LAZY:
		default:
			return lazy(i, j);
		}
	}
};
const mpz_class LasyRecursiveSeq2::DEFAULT = -1;

mpz_class fib(int n, LasyRecursiveSeq & c) {
	//std::cout << "call of " << n << std::endl;
	if (n == 0)
		return 0;
	if (n == 1)
		return 1;
	else
		return c[n - 2] + c[n - 1];
}

mpz_class dellanoi(int n, int m, LasyRecursiveSeq2 & c) {
	if (n == 0 || m == 0)
		return 1;
	return c.at(n - 1, m - 1) + c.at(n, m - 1) + c.at(n - 1, m);
}

mpz_class LukSeqU(int P, int Q, int n, LasyRecursiveSeq & c) {
	if (n <= 1)
		return n;
	return P*c[n - 1] - Q*c[n - 2];
}

int main(){
	setlocale(0, "Ru");
	std::cout << "¬ведите желаемый номер номер числа ...: ";
	int n;
	std::cin >> n;
	LasyRecursiveSeq lrs_fib(fib);
	std::cout << lrs_fib[n] << std::endl;

	LasyRecursiveSeq2 del(dellanoi);
	std::cout << del.lazy(2,n) << std::endl;

	int P, Q;
	std::cin >> P >> Q;
	LasyRecursiveSeq luk([P, Q](int k, LasyRecursiveSeq & c) {return LukSeqU(P, Q, k, c); });
	std::cout << luk[n] << std::endl;
	system("pause");

	return 0;
}
