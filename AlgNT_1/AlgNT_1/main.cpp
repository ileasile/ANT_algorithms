#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <limits>
#include "BigIntException.h"

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
sll abs_num(sll val) {
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
	size_t dig() const {
		return data.size();
	}

	//|a| + |b|
	static BigInt & addAbs(BigInt & a, const BigInt & b);

	// |a| - |b|
	static BigInt & subAbs(BigInt & a, const BigInt & b);

	// a + sign * b
	static BigInt & addSign(BigInt & a, const BigInt & b, char sign);

	

public:
	BigInt(char sgn, const intvec & data) :sgn(sgn), data(data) {}
	BigInt(char sgn, const BigInt & a) :sgn(sgn), data(a.data){}
	BigInt(const BigInt & a) : sgn(a.sgn), data(a.data) {}
	BigInt(ull val, char sign);
	BigInt(sll val = 0L): BigInt (abs_num(val),sign(val)) {}
	BigInt(string val, ui inB = 10);

	friend std::ostream & operator<<(std::ostream & s, const BigInt & a);

public:
	bool isNull() const;
	bool isNeg() const;
	bool isPos() const;
	BigInt abs() const;
	char compareAbs(const BigInt & a) const;
	char compare(const BigInt & a) const;
	bool operator< (const BigInt & a) const;
	bool operator> (const BigInt & a) const;
	bool operator== (const BigInt & a) const;
	bool operator!= (const BigInt & a) const;
	bool operator<= (const BigInt & a) const;
	bool operator>= (const BigInt & a) const;

	BigInt operator + (const BigInt & a) const;
	BigInt operator - (const BigInt & a) const;
	BigInt operator - () const;

	BigInt & operator += (const BigInt & a);
	BigInt & operator -= (const BigInt & a);
	BigInt & operator ++ ();
	BigInt operator ++ (int);
	BigInt & operator -- ();
	BigInt operator -- (int);
	
	// *this * 2^(32n)
	BigInt & big_shift(int n);
	// *this / 2^n
	BigInt operator >> (int n) const;
	// *this * 2^n
	BigInt operator << (int n) const;

	friend BigInt operator * (ui a, const BigInt & b);
	friend BigInt operator * (const BigInt & b, ui a);
	BigInt operator * (const BigInt & a) const;
	BigInt & operator *= (const BigInt & a);
	BigInt & operator *= (const ui a);

	BigInt operator / (const BigInt & d) const;
	BigInt & operator/=(const BigInt & a);

};

BigInt::BigInt(string val, ui inB = 10) {
	if (val.empty()) {
		sgn = 0;
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

	for (auto v : val) {
		*this *= inB;
		*this += digval(v);
	}
	normalize();
}
BigInt::BigInt(ull val, char sign) {
	sgn = val == 0 ? 0 : sign;

	while (val > 0) {
		data.push_back(val - ((val >> SOI) << SOI));
		val >>= SOI;
	}
}

BigInt & BigInt::addAbs(BigInt & a, const BigInt & b) {
	auto it1 = a.data.begin();
	auto it2 = b.data.cbegin();

	ull carry = 0;
	for (;it1 != a.data.end() && it2 != b.data.end(); ++it1, ++it2) {
		ull r = carry + *it1 + *it2;
		if (r > C2_32_1) {
			carry = 1;
			r -= C2_32;
		}
		else {
			carry = 0;
		}
		*it1 = r;
	}

	for (;it1 != a.data.end(); ++it1) {
		ull r = carry + *it1;
		if (r > C2_32_1) {
			carry = 1;
			r -= C2_32;
		}
		else {
			carry = 0;
			break;
		}
		*it1 = r;
	}
	for (;it2 != b.data.end(); ++it2) {
		ull r = carry + *it2;
		if (r > C2_32_1) {
			carry = 1;
			r -= C2_32;
		}
		else {
			carry = 0;
		}
		a.data.push_back(r);
	}
	if (carry > 0) {
		a.data.push_back((ui)carry);
	}
	return a;
}
BigInt & BigInt::subAbs(BigInt & a, const BigInt & b) {
	char sgn = a.compareAbs(b);
	const auto * pa = &a; 
	const auto * pb = &b;
	if (sgn == -1) {
		pa = &b;
		pb = &a;
	}
	auto & ra = *pa;
	auto & rb = *pb;

	auto ita = a.data.begin();
	auto it1 = ra.data.cbegin();
	auto it2 = rb.data.cbegin();

	ull carry = 0;
	for (;it1 != ra.data.end() && it2 != rb.data.end(); ++it1, ++it2, ++ita) {
		ull r = *it1 - *it2 - carry;
		if (r < 0) {
			carry = 1;
			r += C2_32;
		}
		else {
			carry = 0;
		}
		*ita = r;
	}

	if (ita != a.data.end()) {
		for (;ita != a.data.end(); ++ita) {
			ull r = *ita - carry;
			if (r < 0) {
				carry = 1;
				r += C2_32;
			}
			else {
				carry = 0;
			}
			*ita = r;
		}
	}
	else {
		for (;it1 != ra.data.end(); ++it1) {
			ull r = *it1 - carry;
			if (r < 0) {
				carry = 1;
				r += C2_32;
			}
			else {
				carry = 0;
			}
			a.data.push_back(r);
		}
	}

	return a.normalize();
}
BigInt & BigInt::addSign(BigInt & a, const BigInt & b, char sign) {
	if (a.sgn * b.sgn * sign >= 0) {
		if (a.sgn + b.sgn == 0)
			a.sgn = 0;
		return addAbs(a, b);
	}

	if (a.sgn > 0) return subAbs(a, b);
	return subAbs(a, b).negate();
}

bool BigInt::isNull() const {
	return sgn == 0;
}
bool BigInt::isNeg() const {
	return sgn == -1;
}
bool BigInt::isPos() const {
	return sgn == 1;
}
BigInt BigInt::abs() const {
	if (isNeg()) return -*this;
	else return *this;
}
char BigInt::compareAbs(const BigInt & a) const {
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
char BigInt::compare(const BigInt & a) const {
	if (sgn != a.sgn)
		return sgn < a.sgn ? -1 : 1;
	return compareAbs(a);
}
bool BigInt::operator< (const BigInt & a) const {
	return compare(a) == -1;
}
bool BigInt::operator> (const BigInt & a) const {
	return compare(a) == 1;
}
bool BigInt::operator== (const BigInt & a) const {
	return compare(a) == 0;
}
bool BigInt::operator!= (const BigInt & a) const {
	return compare(a) != 0;
}
bool BigInt::operator<= (const BigInt & a) const {
	return compare(a) <= 0;
}
bool BigInt::operator>= (const BigInt & a) const {
	return compare(a) >= 0;
}

BigInt BigInt::operator + (const BigInt & a) const {
	auto r = *this;
	return r += a;
}
BigInt BigInt::operator - (const BigInt & a) const {
	auto r = *this;
	return r -= a;
}
BigInt BigInt::operator - () const {
	BigInt a(*this);
	a.negate();
	return a;
}

BigInt & BigInt::operator += (const BigInt & a) {
	return addSign(*this, a, 1);
}
BigInt & BigInt::operator -= (const BigInt & a) {
	return addSign(*this, a, -1);
}
BigInt & BigInt::operator ++ () {
	return addSign(*this, 1, 1);
}
BigInt BigInt::operator ++ (int) {
	auto a = *this;
	++*this;
	return a;
}
BigInt & BigInt::operator -- () {
	return addSign(*this, 1, -1);
}
BigInt BigInt::operator -- (int) {
	auto a = *this;
	--*this;
	return a;
}

BigInt & BigInt::big_shift(int n) {
	data.insert(data.begin(), n, 0);
	return *this;
}

// *this / 2^n
BigInt BigInt::operator >> (int n) const {
	int skip = n / SOI;
	int k = n % SOI;
	int soi_k = SOI - k;
	ui lowest = (1 << k) - 1;
	ui hb = 0, lb = 0;

	intvec newdata;
	auto it = data.begin() + skip;
	if (it != data.end())
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
BigInt BigInt::operator << (int n) const {
	if (isNull()) return *this;

	int skip = n / SOI;
	int k = n % SOI;
	int soi_k = SOI - k;
	ui lowest = (1 << soi_k) - 1;
	ui hb = 0, lb = 0;

	intvec newdata(skip);
	for (auto it = data.begin();it != data.end();++it) {
		lb = *it & lowest;
		newdata.push_back(hb | (lb << k));
		hb = *it >> soi_k;
	}
	newdata.push_back(hb);
	return BigInt(sgn, newdata);
}

std::ostream & operator<<(std::ostream & s, const BigInt & a){

	return s;
}

BigInt operator * (ui a, const BigInt & b) {
	auto it = b.data.cbegin();

	ull carry = 0;
	intvec newdata;
	for (;it != b.data.end(); ++it) {
		ull r = carry + *it * (ull)a;
		carry = r >> SOI;
		r -= carry << SOI;
		newdata.push_back(r);
	}

	if (carry > 0) {
		newdata.push_back((ui)carry);
	}
	return BigInt(b.sgn, newdata).normalize();
}
BigInt operator * (const BigInt & b, ui a) {
	return a*b;
}
BigInt BigInt::operator * (const BigInt & a) const  {
	BigInt res = 0;

	for (size_t i = 0; i < data.size(); ++i) {
		auto toadd = data[i] * a;
		toadd.big_shift(i);
		res += toadd;
	}

	return res;
}
BigInt & BigInt::operator *= (const BigInt & a) {
	return *this = *this * a;
}
BigInt & BigInt::operator *= (const ui a)
{
	return *this = *this * a;
}

BigInt BigInt::operator / (const BigInt & d) const {
	if (d.isNull()) {
		throw BigIntDivideByZeroException();
	}
	if (this->isNull()) {
		return 0;
	}
	char s = this->sgn * d.sgn;
	BigInt a = this->abs(), b = d.abs();

	// this = d*x, x-?
	// Let's find x using binary search
	// We'll solve the following optimization problem:
	// this - d * x -> min
	// (this >= d*x, x>=0)
	// x should be in segment [B^(dig(this)-dig(b)-1), B^(dig(this)-dig(b))]
	size_t da = a.dig(), db = b.dig();
	BigInt A = ((BigInt)0);
	BigInt B = ((BigInt)1).big_shift(da - db);
	while (B > A + 1) {
		BigInt M = (A + B) >> 1;
		if ((a - M * b).isNeg()) {
			B = M;
		}
		else {
			A = M;
		}
	}
	A.sgn = s;
	return A;
}
BigInt & BigInt::operator /= (const BigInt & a) {
	return *this = *this / a;
}

int main() {
	BigInt b(2123);
	(BigInt)123 + b;
}