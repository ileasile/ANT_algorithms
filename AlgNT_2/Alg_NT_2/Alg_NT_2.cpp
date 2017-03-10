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
	const vec & get_data() const{
		return data;
	}

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

mpz_class shcroder_hipparhus(int n, LasyRecursiveSeq & c) {
	if (n <= 2)
		return 1;
	return ((6 * n - 9)*c[n - 1] - (n - 3)*c[n - 2]) / n;
}

int main(){
	setlocale(0, "Ru");
	std::cout << "¬ведите количество выводимых чисел ЎрЄдера-√иппарха: ";
	int n;
	std::cin >> n;

	LasyRecursiveSeq c(shcroder_hipparhus);
	c.straight(n);
	auto v = c.get_data();
	for (int i = 1; i < v.size(); ++i) {
		std::cout <<"a_"<< i <<" = "<< v[i] << std::endl;
	}
	std::cout << std::endl;
	system("pause");

	return 0;
}
