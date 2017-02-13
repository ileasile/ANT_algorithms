#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <limits>

using std::string;

typedef long long sll;
typedef  unsigned long long ull;
typedef unsigned int ui;
typedef unsigned char uc;

const uc SOI = sizeof(ui) * 8;
const ull C2_32 = (ull)std::numeric_limits<ui>::max() + 1;
const ull C2_32_1 = (ull)std::numeric_limits<ui>::max();
typedef std::vector<ui> intvec;

char sign(sll val) {
	return (val > 0) ? 1 : ((val == 0) ? 0 : -1);
}
sll abs(sll val) {
	return (val > 0) ? val : -val;
}
ui log2(ui b) {
	ui ret = 0;
	while (b > 1) {
		b >>= 1;
		++ret;
	}
	return ret;
}
ui digval(char digit) {
	if (digit >= '0' && digit <= '9') {
		return digit - '0';
	}
	if (digit >= 'a' && digit <= 'z') {
		return digit - 'a' + 10;
	}
	if (digit >= 'A' && digit <= 'Z') {
		return digit - 'A' + 10;
	}
}

class BigInt {
	char sgn;
	intvec data;

	BigInt & negate() {
		sgn = -sgn;
		return *this;
	}
	BigInt & normalize() {
		for (auto it = data.rbegin(); it != data.rend() || data.size() < 2; ++it) {
			if (*it != 0)
				break;
			data.erase(std::next(it).base);
		}
		return *this;
	}

	//|a| + |b|
	static BigInt addAbs(const BigInt & a, const BigInt & b);

	// |a| - |b|
	static BigInt subAbs(const BigInt & a, const BigInt & b);

	// a + sign * b
	static BigInt addSign(const BigInt & a, const BigInt & b, char sign);

	BigInt(char sgn, const intvec & data) :sgn(sgn), data(data) {}
	BigInt(char sgn, const BigInt & a) :sgn(sgn), data(a.data){}
	BigInt(const BigInt & a) : sgn(a.sgn), data(a.data) {}

public:
	BigInt(ull val, char sign) {
		sgn = val == 0 ? 0 : sign;

		while (val > 0) {
			data.push_back(val - ((val >> SOI) << SOI));
			val >>= SOI;
		}
	}
	BigInt(sll val = 0L): BigInt(abs(val),sign(val)) {}
	BigInt(string val, ui inB = 10) {
		if (val.empty()) {
			data.push_back(0);
			return;
		}

		sgn = 1;
		if (val[0] == '-') {
			sgn = -1;
			val = val.substr(1);
		}
		if (val[0] == '+') {
			val = val.substr(1);
		}
	}

public:
	bool isNull() const {
		return sgn == 0;
	}
	bool isNeg() const{
		return sgn == -1;
	}
	char compareAbs(const BigInt & a) const {
		if (data.size() != a.data.size()) {
			return data.size() < a.data.size() ? -1 : 1;
		}
		for (size_t i = data.size() - 1; i >= 0; --i) {
			if (data[i] != a.data[i]) {
				return data[i] < a.data[i] ? -1 : 1;
			}
		}
		return 0;
	}
	char compare(const BigInt & a) const {
		if (sgn != a.sgn)
			return sgn < a.sgn ? -1 : 1;
		return compareAbs(a);
	}
	bool operator< (const BigInt & a) const{
		return compare(a) == -1;
	}
	bool operator> (const BigInt & a) const{
		return compare(a) == 1;
	}
	bool operator== (const BigInt & a) const{
		return compare(a) == 0;
	}
	bool operator!= (const BigInt & a) const{
		return compare(a) != 0;
	}
	bool operator<= (const BigInt & a) const{
		return compare(a) <= 0;
	}
	bool operator>= (const BigInt & a) const{
		return compare(a) >= 0;
	}

	BigInt operator + (const BigInt & a) const{
		return addSign(*this, a, 1);
	}
	BigInt operator - (const BigInt & a) const{
		return addSign(*this, a, -1);
	}
	BigInt operator - () const {
		BigInt a(*this);
		a.negate();
		return a;
	}

	BigInt & operator += (const BigInt & a) {
		return *this = *this + a;
	}
	BigInt & operator -= (const BigInt & a) {
		return *this = *this - a;
	}
	BigInt & operator ++ () {
		return *this = *this + 1;
	}
	BigInt operator ++ (int) {
		auto a = *this;
		*this = *this + 1;
		return a;
	}
	BigInt & operator -- () {
		return *this = *this - 1;
	}
	BigInt operator -- (int) {
		auto a = *this;
		*this = *this - 1;
		return a;
	}
	
	// *this * 2^(32n)
	void big_shift(int n) {
		data.insert(data.begin(), n, 0);
	}

	// *this / 2^n
	BigInt operator >> (int n) const {
		int skip = n / SOI;
		int k = n % SOI;
		int soi_k = SOI - k;
		ui lowest = (1 << k) - 1;
		ui hb = 0, lb = 0;

		intvec newdata;
		auto it = data.begin() + skip;
		if(it != data.end())
			for (;;) {
				hb = *it >> k;
				newdata.push_back(hb);

				++it;
				if (it == data.end())
					break;

				lb = *it & lowest;
				newdata.back() = newdata.back() | (lb << soi_k);
			
			}

		return BigInt(sgn, newdata);
	}

	// *this * 2^n
	BigInt operator << (int n) const {
		if (isNull()) return *this;

		int skip = n / SOI;
		int k = n % SOI;
		int soi_k = SOI - k;
		ui lowest = (1 << soi_k) - 1;
		ui hb = 0, lb = 0;

		intvec newdata(skip);
		for (auto it = data.begin();it != data.end();++it) {
			lb = *it & lowest;
			newdata.push_back(hb | (lb << k) );
			hb = *it >> soi_k;
		}
		newdata.push_back(hb);
		return BigInt(sgn, newdata);
	}

	friend BigInt operator * (ui a, const BigInt & b) {
		auto it = b.data.cbegin();

		ull carry = 0;
		intvec newdata;
		for (;it != data.end(); ++it) {
			ull r = carry + *it * (ull) a;
			carry = r >> SOI;
			r -= carry << SOI;
			newdata.push_back(r);
		}

		if (carry > 0) {
			newdata.push_back((ui)carry);
		}
		return BigInt(b.sgn, newdata).normalize();
	}
	friend BigInt operator * ( const BigInt & b, ui a) {
		return a*b;
	}

	BigInt operator * (const BigInt & a) {
		BigInt res = 0;

		for (size_t i = 0; i < data.size(); ++i) {
			auto toadd = data[i] * a;
			toadd.big_shift(i);
			res += toadd;
		}

		return res;
	}

};

BigInt BigInt::addAbs(const BigInt & a, const BigInt & b) {
	auto it1 = a.data.cbegin();
	auto it2 = b.data.cbegin();

	ull carry = 0;
	intvec newdata;
	for (;it1 != a.data.end() && it2 != b.data.end(); ++it1, ++it2) {
		ull r = carry + *it1 + *it2;
		if (r > C2_32_1) {
			carry = 1;
			r -= C2_32;
		}
		else {
			carry = 0;
		}
		newdata.push_back(r);
	}

	auto data_end = a.data.end();
	if (it1 == a.data.end()) {
		it1 = it2;
		data_end = b.data.end();
	}
	for (;it1 != data_end; ++it1) {
		ull r = carry + *it1;
		if (r > C2_32_1) {
			carry = 1;
			r -= C2_32;
		}
		else {
			carry = 0;
		}
		newdata.push_back(r);
	}
	if (carry > 0) {
		newdata.push_back((ui)carry);
	}
	return BigInt(1, newdata);
}

BigInt BigInt::subAbs(const BigInt & a, const BigInt & b) {
	char sgn = a.compareAbs(b);
	auto * pa = &a, * pb = &b;
	if (sgn == -1) {
		pa = &b;
		pb = &a;
	}
	auto & ra = *pa;
	auto & rb = *pb;

	auto it1 = ra.data.cbegin();
	auto it2 = rb.data.cbegin();

	ull carry = 0;
	intvec newdata;
	for (;it1 != ra.data.end() && it2 != rb.data.end(); ++it1, ++it2) {
		ull r = *it1 - *it2 - carry;
		if (r < 0) {
			carry = 1;
			r += C2_32;
		}
		else {
			carry = 0;
		}
		newdata.push_back(r);
	}

	for (;it1 != a.data.end(); ++it1) {
		ull r = *it1 - carry;
		if (r < 0) {
			carry = 1;
			r += C2_32;
		}
		else {
			carry = 0;
		}
		newdata.push_back(r);
	}

	BigInt res(sgn, newdata);
	return res.normalize();
}

BigInt BigInt::addSign(const BigInt & a, const BigInt & b, char sign) {
	if (a.sgn * b.sgn * sign >= 0)
		return BigInt(a.sgn + b.sgn == 0 ? 0 : a.sgn, addAbs(a, b));

	BigInt res = subAbs(a, b);
	if (a.sgn > 0) return res;
	return res.negate();
}

int main() {
	BigInt b(2123);
	(BigInt)123 + b;
}