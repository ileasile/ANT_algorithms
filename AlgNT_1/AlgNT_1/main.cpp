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

	void negate() {
		sgn = -sgn;
	}

	void normalize() {
		for (auto it = data.rbegin(); it != data.rend() || data.size() < 2; ++it) {
			if (*it != 0)
				break;
			data.erase(std::next(it).base);
		}
	}

	//|a|+|b|
	static BigInt addAbs(const BigInt & a, const BigInt & b) {
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
		return BigInt(1, a.base, newdata);
	}

	// |a| - |b|
	friend BigInt subAbs(const BigInt & a, const BigInt & b);

	BigInt(char sgn, const intvec & data) :sgn(sgn), data(data) {}
	BigInt(char sgn, const BigInt & a) :sgn(sgn), data(a.data){}

public:
	BigInt(sll val = 0L) {
		sgn = sign(val);
		val = abs(val);

		while (val > 0) {
			data.push_back(val - ((val >> SOI) << SOI));
			val >>= SOI;
		}

		if (sgn == -1) {
			negate();
		}
	}
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
	bool isNeg() const{
		if (sgn == -1)
			return true;
		return false;
	}
	bool operator< (const BigInt & a) const{
		if (sgn != a.sgn)
			return sgn < a.sgn;
		if (data.size() != a.data.size()){
			return isNeg() ^ this->data.size() < a.data.size();
		}
		for (size_t i = data.size() - 1; i >= 0; --i){
			if (data[i] != a.data[i]){
				return isNeg() ^ data[i] < a.data[i];
			}
		}
		return false;
	}
	bool operator> (const BigInt & a) const{
		if (sgn != a.sgn)
			return sgn > a.sgn;
		if (data.size() != a.data.size()){
			return isNeg() ^ this->data.size() > a.data.size();
		}
		for (size_t i = data.size() - 1; i >= 0; --i){
			if (data[i] != a.data[i]){
				return isNeg() ^ data[i] > a.data[i];
			}
		}
		return false;
	}
	bool operator== (const BigInt & a) const{
		if (sgn != a.sgn)
			return false;
		return data == a.data;
	}
	bool operator!= (const BigInt & a) const{
		if (sgn != a.sgn)
			return true;
		return data != a.data;
	}
	bool operator<= (const BigInt & a) const{
		if (sgn != a.sgn)
			return sgn < a.sgn;
		if (data.size() != a.data.size()){
			return isNeg() ^ this->data.size() < a.data.size();
		}
		for (size_t i = data.size() - 1; i >= 0; --i){
			if (data[i] != a.data[i]){
				return  isNeg() ^ data[i] < a.data[i];
			}
		}
		return true;
	}
	bool operator>= (const BigInt & a) const{
		if (sgn != a.sgn)
			return sgn > a.sgn;
		if (data.size() != a.data.size()){
			return isNeg() ^ this->data.size() > a.data.size();
		}
		for (size_t i = data.size() - 1; i >= 0; --i){
			if (data[i] != a.data[i]){
				return isNeg() ^ data[i] > a.data[i];
			}
		}
		return true;
	}

	BigInt operator + (const BigInt & a) const{
		if (sgn * a.sgn >= 0)
			return BigInt(sgn, addAbs(*this, a));
		else{

		}
	}

	BigInt operator - (const BigInt & a) {

	}

	BigInt operator - () {
		BigInt a(*this);
		a.negate();
		return a;
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
		return BigInt(b.base, newdata);
	}
	friend BigInt operator * ( const BigInt & b, ui a) {
		return a*b;
	}
};

BigInt subAbs(const BigInt & a, const BigInt & b) {

}