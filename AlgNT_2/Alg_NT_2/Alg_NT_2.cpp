#include <mpirxx.h>
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>

class LasyRecursiveSeq
{
	typedef std::function<mpz_class(int, LasyRecursiveSeq &)> func_type;
	typedef std::vector<mpz_class> vec;
	static const mpz_class DEFAULT;

	func_type func;
	vec data;

	void resize(int i) {
		if (i >= data.size()) {
			data.resize(i+1, DEFAULT);
		}
	}
public:
	LasyRecursiveSeq(func_type func) :func(func) {}
	//may cause stack overflow
	mpz_class lazy(int i) {
		if (i < data.size() && data[i] != DEFAULT) {
			return data[i];
		}
		resize(i);
		return data[i] = func(i, *this);
	}
	//sholdn't cause stack overflow
	mpz_class operator[](int i) {
		if (i < data.size() && data[i] != DEFAULT) {
			return data[i];
		}
		resize(i);
		for (int j = 0; j <= i; ++j) {
			data[j] = func(j, *this);
		}
		return data[i];
	}
private:

};
const mpz_class LasyRecursiveSeq::DEFAULT = -1;

class LasyRecursiveSeq2
{
	typedef std::function<mpz_class(int, int, LasyRecursiveSeq2 &)> func_type;
	typedef std::vector<std::vector<mpz_class>> vec;
	static const mpz_class DEFAULT;

	func_type func;
	vec data;

	void resize(int i, int j) {
		if (i >= data.size()) {
			data.resize(i + 1);
		}
		for (int k = 0; k <= i; ++k) {
			if(j >= data[k].size())
				data[k].resize(j + 1, DEFAULT);
		}
	}
public:
	LasyRecursiveSeq2(func_type func) :func(func) {}
	//may cause stack overflow
	mpz_class lazy(int i, int j) {
		if (i < data.size() && j < data[i].size() && data[i][j] != DEFAULT) {
			return data[i][j];
		}
		resize(i, j);
		return data[i][j] = func(i, j, *this);
	}
	//sholdn't cause stack overflow
	mpz_class at(int i, int j) {
		if (i < data.size() && j < data[i].size() && data[i][j] != DEFAULT) {
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
private:

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

int main(){
	setlocale(0, "Ru");
	std::cout << "¬ведите желаемый номер номер числа ...: ";
	int n;
	std::cin >> n;
	LasyRecursiveSeq lrs_fib(fib);
	std::cout << lrs_fib[n] << std::endl;

	LasyRecursiveSeq2 del(dellanoi);
	std::cout << del.lazy(2,n) << std::endl;

	system("pause");

	return 0;
}
