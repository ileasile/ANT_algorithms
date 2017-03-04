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
public:
	LasyRecursiveSeq(func_type func) :func(func) {}
	//may cause stack overflow
	mpz_class lazy(int i) {
		if (i < data.size() && data[i] != DEFAULT) {
			return data[i];
		}
		if (i >= data.size()) {
			data.resize(i+1, DEFAULT);
		}
		return data[i] = func(i, *this);
	}
	//sholdn't cause stack overflow
	mpz_class operator[](int i) {
		if (i < data.size() && data[i] != DEFAULT) {
			return data[i];
		}
		if(i >= data.size())
			data.resize(i + 1, DEFAULT);
		for (int j = 0; j <= i; ++j) {
			data[j] = func(j, *this);
		}
		return data[i];
	}
private:

};

const mpz_class LasyRecursiveSeq::DEFAULT = -1;

mpz_class fib(int n, LasyRecursiveSeq & c) {
	//std::cout << "call of " << n << std::endl;
	if (n == 0)
		return 0;
	if (n == 1)
		return 1;
	else
		return c[n - 2] + c[n - 1];
}

int main(){
	LasyRecursiveSeq lrs_fib(fib);

	std::cout << lrs_fib[100000] << std::endl;

	system("pause");

	return 0;
}
