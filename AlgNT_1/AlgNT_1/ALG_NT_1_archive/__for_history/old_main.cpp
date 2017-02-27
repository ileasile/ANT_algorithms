#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <limits>
#include <utility>
#include <sstream>
#include <algorithm>
#include <tuple>
#include "BigIntException.h"


class BigInt {
public:
	typedef uint16_t	bui;
	typedef int16_t		bsi;
	typedef uint32_t	lui;
	typedef int32_t		lsi;
	typedef uint64_t	llui;
	typedef	int64_t		llsi ;

	typedef std::pair<BigInt, BigInt> QuRem;
	typedef std::vector<bui> intvec;

	static const unsigned char SOI = sizeof(bui) * 8;
	static const unsigned char SOI_1 = SOI - 1;
	static const unsigned char LOG_SOI = 1 + 3; // log(16) = log(2) + log(8)
	static const lui C_MAX_DIG_1 = (lui)std::numeric_limits<bui>::max() + 1;
	static const lui C_MAX_DIG = (lui)std::numeric_limits<bui>::max();

	template<typename signed_int>
	static char sign(signed_int val) {
		return (val > 0) ? 1 : ((val == 0) ? 0 : -1);
	}

	template<typename signed_int>
	static signed_int abs_num(signed_int val) {
		return (val > 0) ? val : -val;
	}

	template<typename unsigned_int>
	static unsigned char log2(unsigned_int b) {
		unsigned char ret = 0;
		while (b > 1) {
			b >>= 1;
			++ret;
		}
		return ret;
	}

	static unsigned char digval(char digit) {
		if (digit >= '0' && digit <= '9') {
			return digit - '0';
		}
		if (digit >= 'a' && digit <= 'z') {
			return digit - 'a' + 10;
		}
		if (digit >= 'A' && digit <= 'Z') {
			return digit - 'A' + 10;
		}
		throw BigIntIllegalDigitException();
	}

	static std::string dig_by_val(bui val) {
		if (val >= 0 && val <= 9) {
			return std::string(1, '0' + val);
		}
		if (val >= 10 && val <= 35) {
			return std::string(1, 'A' + val - 10);
		}
		return "(" + std::to_string(val) + ")";
	}

	static std::pair<llsi, llsi> quo_rem(llsi a, llsi b) {
		///TODO: remake with div()
		auto q = a / b;
		auto r = a % b;
		if (q >= 0)
			return std::make_pair(q, r);
		else
			return std::make_pair(q - 1, r + b);
	}
	static std::pair<llui, llui> quo_rem(llui a, llui b) {
		///TODO: remake with div()
		return std::make_pair(a / b, a%b);
	}

private:
	char sgn;
	intvec data;

	BigInt & negate();
	BigInt & normalize();
	size_t dig() const;

	//|a| + |b|
	static BigInt & addAbs(BigInt & a, const BigInt & b);

	// |a| - |b|
	static BigInt & subAbs(BigInt & a, const BigInt & b);

	// a + sign * b
	static BigInt & addSign(BigInt & a, const BigInt & b, char sign);

	bui & operator[] (size_t i) {
		return data[i];
	}
	const bui & operator[] (size_t i) const{
		return data[i];
	}

public:
	BigInt(char sgn, const intvec & data) :sgn(sgn), data(data) {}
	BigInt(char sgn, const BigInt & a) :sgn(sgn), data(a.data){}
	BigInt(const BigInt & a) : sgn(a.sgn), data(a.data) {}
	BigInt(unsigned long long val, char sign);
	BigInt(signed long long val = 0L): BigInt (abs_num(val),sign(val)) {}
	BigInt(std::string val, unsigned inB = BigInt::inputBase);

	template <class inttype>
	BigInt(const std::vector<inttype> & v, char sign = 1);

	template<class inttype>
	friend std::vector<inttype> & copy(std::vector<inttype> & v, const BigInt & a);

	static unsigned inputBase;
	static unsigned outputBase;
	static bool printPlus;

	
	friend std::ostream & operator<<(std::ostream & s, const BigInt & a);
	std::string to_string(unsigned base = BigInt::outputBase) const;

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
	BigInt & operator >>= (int n);
	// *this * 2^n
	BigInt operator << (int n) const;
	BigInt & operator <<= (int n);

	friend BigInt operator * (bui a, const BigInt & b);
	friend BigInt operator * (const BigInt & b, bui a);
	BigInt operator * (const BigInt & a) const;
	BigInt & operator *= (const BigInt & a);
	BigInt & operator *= (const bui a);

	QuRem div(const BigInt & d) const;
	BigInt operator / (const BigInt & d) const;
	BigInt operator % (const BigInt & d) const;
	BigInt & operator/=(const BigInt & a);

};

template<class inttype>
std::vector<inttype> & copy(std::vector<inttype> & v, const BigInt & a) {
	v.resize(a.dig());
	for (size_t i = 0; i < v.size(); ++i) {
		v[i] = a[i];
	}
	return v;
}

unsigned BigInt::inputBase = 10;
unsigned BigInt::outputBase = 10;
bool BigInt::printPlus = false;

BigInt::BigInt(std::string val, unsigned inB) {
	if (val.empty()) {
		sgn = 0;
		return;
	}

	char sgn = 1;
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
	this->sgn = sgn;
	normalize();
}
BigInt::BigInt(unsigned long long val, char sign) {
	sgn = val == 0 ? 0 : sign;

	while (val > 0) {
		data.push_back((bui)(val & C_MAX_DIG));
		val >>= SOI;
	}
}
template<class inttype>
BigInt::BigInt(const std::vector<inttype>& v, char sign) {
	data.resize(v.size());
	auto itv = v.cbegin();
	for (auto it = data.begin(); it != data.end(); ++it, ++itv) {
		*it = (bui)*itv;
	}
	normalize();
	sgn = sign;
	if (data.size() == 0) {
		sgn = 0;
	}
}

BigInt & BigInt::addAbs(BigInt & a, const BigInt & b) {
	if (b.isNull())
		return a;
	
	auto it1 = a.data.begin();
	auto it2 = b.data.cbegin();

	lui carry = 0;
	for (;it1 != a.data.end() && it2 != b.data.end(); ++it1, ++it2) {
		lui r = carry + *it1 + *it2;
		if (r > C_MAX_DIG) {
			carry = 1;
			r &= C_MAX_DIG;
		}
		else {
			carry = 0;
		}
		*it1 = (bui)r;
	}

	for (;it1 != a.data.end(); ++it1) {
		lui r = carry + *it1;
		if (r > C_MAX_DIG) {
			carry = 1;
			r &= C_MAX_DIG;
		}
		else {
			carry = 0;
			break;
		}
		*it1 = (bui)r;
	}
	for (;it2 != b.data.end(); ++it2) {
		lui r = carry + *it2;
		if (r > C_MAX_DIG) {
			carry = 1;
			r &= C_MAX_DIG;
		}
		else {
			carry = 0;
		}
		a.data.push_back((bui)r);
	}
	if (carry > 0) {
		a.data.push_back((bui)carry);
	}

	a.sgn = 1;

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

	lsi carry = 0;
	for (;it1 != ra.data.end() && it2 != rb.data.end(); ++it1, ++it2, ++ita) {
		lsi r = - carry + *it1 - *it2 ;
		if (r < 0) {
			carry = 1;
			r += C_MAX_DIG_1;
		}
		else {
			carry = 0;
		}
		*ita = (bui)r;
	}

	if (ita != a.data.end()) {
		for (;ita != a.data.end(); ++ita) {
			lsi r = - carry + *ita ;
			if (r < 0) {
				carry = 1;
				r += C_MAX_DIG_1;
			}
			else {
				carry = 0;
			}
			*ita = (bui)r;
		}
	}
	else {
		for (;it1 != ra.data.end(); ++it1) {
			lsi r = - carry + *it1 ;
			if (r < 0) {
				carry = 1;
				r += C_MAX_DIG_1;
			}
			else {
				carry = 0;
			}
			a.data.push_back((bui)r);
		}
	}
	a.sgn = sgn;
	return a.normalize();
}
BigInt & BigInt::addSign(BigInt & a, const BigInt & b, char sign) {
	if (a.sgn * b.sgn * sign >= 0) {
		char sgn;
		if (a.sgn + sign*b.sgn == 0)
			sgn = 0;
		else if (a.sgn != 0)
			sgn = a.sgn;
		else
			sgn = sign * b.sgn;
		addAbs(a, b);
		a.sgn = sgn;
		return a;
	}

	if (a.sgn > 0) return subAbs(a, b);
	return subAbs(a, b).negate();
}

std::ostream & operator<<(std::ostream & s, const BigInt & a) {
	return s << a.to_string();
}

std::string BigInt::to_string(unsigned base) const{
	std::vector<std::string> out;
	std::ostringstream s;
	if (isNeg() || BigInt::printPlus)
		s << (isNeg() ? '-' : '+');
	if (isNull()) {
		out.push_back("0");
	}
	else {
		BigInt A = abs(), R;
		while (A > 0) {
			std::tie(A, R) = A.div(base);
			if (R.isNull())
				out.push_back("0");
			else {
				out.push_back(BigInt::dig_by_val(R.data[0]));
			}
		}
	}
	for (auto it = out.rbegin(); it != out.rend(); ++it) {
		s << *it;
	}
	return s.str();
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
BigInt & BigInt::negate() {
	sgn = -sgn;
	return *this;
}
BigInt & BigInt::normalize() {
	int i = data.size() - 1;
	for (; i >= 0; --i) {
		if (data[i] != 0)
			break;
	}
	data.resize(i + 1);
	if (data.size() == 0) sgn = 0;
	return *this;
}
size_t BigInt::dig() const {
	return data.size();
}


char BigInt::compareAbs(const BigInt & a) const {
	if (data.size() != a.data.size()) {
		return data.size() < a.data.size() ? -1 : 1;
	}
	for (int i = data.size() - 1; i >= 0; --i) {
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
	if (n >= 0)
		data.insert(data.begin(), n, 0);
	else
		data.erase(data.begin(), data.begin() - n);
	return *this;
}

// *this / 2^n
BigInt BigInt::operator >> (int n) const {
	auto a(*this);
	return a >>= n;
}

// *this * 2^n
BigInt BigInt::operator << (int n) const {
	auto a(*this);
	return a <<= n;
}

BigInt & BigInt::operator >>= (int n) {
	int skip = n >> LOG_SOI;
	int k = n & SOI_1;
	int soi_k = SOI - k;
	bui lowest = (1 << k) - 1;
	bui hb = 0, lb = 0;

	big_shift(-skip);
	auto it = data.begin();
	if (it != data.end())
		for (;;) {
			hb = *it >> k;
			*it = hb;

			++it;
			if (it == data.end())
				break;

			lb = *it & lowest;
			*(it-1) |= (lb << soi_k);

		}

	return normalize();
}
BigInt & BigInt::operator <<= (int n) {
	if (isNull()) return *this;

	int skip = n >> LOG_SOI;
	int k = n & SOI_1;
	int soi_k = SOI - k;
	bui lowest = (1 << soi_k) - 1;
	bui hb = 0, lb = 0;

	big_shift(skip);
	for (auto it = data.begin();it != data.end();++it) {
		auto val = *it;
		lb = val & lowest;
		*it = hb | (lb << k);
		hb = val >> soi_k;
	}
	if(hb)
		data.push_back(hb);
	return normalize();
}

BigInt operator * (BigInt::bui a, const BigInt & b) {
	auto it = b.data.cbegin();

	BigInt::lui carry = 0;
	BigInt::intvec newdata;
	for (;it != b.data.end(); ++it) {
		BigInt::lui r = carry + *it * (BigInt::lui)a;
		carry = r >> BigInt::SOI;
		r &= BigInt::C_MAX_DIG;
		newdata.push_back((BigInt::bui)r);
	}

	if (carry > 0) {
		newdata.push_back((BigInt::bui)carry);
	}
	return BigInt(a==0 ? 0 : b.sgn, newdata).normalize();
}
BigInt operator * (const BigInt & b, BigInt::bui a) {
	return a*b;
}
BigInt BigInt::operator * (const BigInt & a) const  {
	BigInt res = 0;

	for (size_t i = 0; i < data.size(); ++i) {
		auto toadd = data[i] * a;
		toadd.big_shift(i);
		res += toadd;
	}
	if (sgn == -1)
		res.sgn = -res.sgn;
	return res;
}
BigInt & BigInt::operator *= (const BigInt & a) {
	return *this = *this * a;
}
BigInt & BigInt::operator *= (const bui a)
{
	return *this = *this * a;
}

BigInt::QuRem BigInt::div(const BigInt & d) const
{
	/*
	if (d.isNull()) {
		throw BigIntDivideByZeroException();
	}
	if (this->isNull()) {
		return (QuRem)std::make_pair(0,0);
	}

	// this = d*x, x-?
	// Let's find x using binary search
	// We'll solve the following optimization problem:
	// this - d * x -> min
	// (this >= d*x, x>=0)
	// x should be in segment [B^(dig(this)-dig(b)-1), B^(dig(this)-dig(b))]
	if (compareAbs(d) == -1)
		return std::make_pair(0, *this);

	size_t da = this->dig(), db = d.dig();
	BigInt B = ((BigInt)1).big_shift(da - db + 1);
	BigInt A = -B;
	BigInt R;
	while (B > A + 1) {
		BigInt M = (A + B) >> 1;
		R = *this - M*d;
		if (R.isNeg() ^ d.isNeg()) {
			B = M;
		}
		else {
			A = M;
		}
	}
	
	BigInt RA, RB;
	RA = *this - A*d;
	RB = *this - B*d;
	R = RA.isNeg() ? RB : RA;
	return std::make_pair(A,R);
	*/

	/*
	if (d.isNull()) {
		throw BigIntDivideByZeroException();
	}
	if (this->isNull()) {
		return (QuRem)std::make_pair(0, 0);
	}

	char res_sgn = sgn * d.sgn;
	int bits_shift = 0;
	auto A = this->abs(), B = d.abs();

	std::vector<llsi> q;
	std::vector<llsi> r;
	if (A.compareAbs(B) == -1) {
		copy(r, *this);
	}
	else {
		auto eldest_dig = B.data.back();
		bits_shift = SOI - log2(eldest_dig) - 1;
		A <<= bits_shift;
		B <<= bits_shift;

		auto k = A.dig(), l = B.dig();
		copy(r, A);
		r.push_back(0);
		q.resize(k - l + 1);

		for (int i = k - l; i >= 0; --i) {
			q[i] = ( (r[i + l] << SOI) + r[i + l - 1]) / (llsi) B[l - 1];
			if (q[i] > C_MAX_DIG) 
				q[i] = C_MAX_DIG;
			llsi carry = 0;
			for (size_t j = 0; j < l; ++j) {
				llsi tmp = r[i + j] - q[i] * (llsi)B[j] + carry;
				//std::tie(carry, r[i + j]) = quo_rem(tmp, C_MAX_DIG_1);
				carry = tmp / C_MAX_DIG_1;
				r[i + j] = tmp % C_MAX_DIG_1;
			}
			r[i + l] += carry;
			while (r [i + l] < 0)
			{
				carry = 0;
				for (size_t j = 0; j < l; ++j) {
					llsi tmp = r[i + j] + (llsi)B[j] + carry;
					//std::tie(carry, r[i + j]) = quo_rem(tmp, C_MAX_DIG_1);
					carry = tmp / C_MAX_DIG_1;
					r[i + j] = tmp % C_MAX_DIG_1;
				}
				r[i + l] += carry;
				-- q[i];
			}
		}
		r.resize(l);
	}
	
	BigInt Q(q), R(r);
	R >>= bits_shift;
	if (res_sgn == -1) {
		Q.negate();
		R.negate();
		-- Q;
		R += B;
	}

	return std::make_pair(Q, R);
	*/

	if (d.isNull()) {
		throw BigIntDivideByZeroException();
	}
	if (this->isNull()) {
		return (QuRem)std::make_pair(0, 0);
	}

	char res_sgn = sgn * d.sgn;
	int bits_shift = 0;
	auto A = this->abs(), B = d.abs();
	BigInt Q, R;

	while (A > B) {
		B <<= 1;
		++bits_shift;
	}
	while (A < B) {
		B >>= 1;
		--bits_shift;
	}
	if (bits_shift >= 0) {
		for (int i = 0; i <= bits_shift; ++i) {
			if (B <= A) {
				A -= B;
				B >>= 1;
				Q <<= 1;
				++Q;
			}
			else {
				B >>= 1;
				Q <<= 1;
			}
		}
	}
	R = A;
	
	if (res_sgn == -1) {
		Q.negate();
		R.negate();
		--Q;
		R += B;
	}
	
	Q.normalize();
	R.normalize();
	return std::make_pair(Q, R);
	
}

BigInt BigInt::operator / (const BigInt & d) const {
	return div(d).first;
}
BigInt BigInt::operator %(const BigInt & d) const
{
	return div(d).second;
}
BigInt & BigInt::operator /= (const BigInt & a) {
	return *this = *this / a;
}

int main() {
	BigInt b(2123);
	BigInt rsum = (BigInt)123 + b;
	std::cout << rsum <<"\n";

	BigInt s(1048576);
	for (int i = 0; i < 65; ++i) {
		std::cout << s <<"\n";
		s <<= 1;
	}

	BigInt bigval("256347236821592549245621342683256565878543255325353546575474575685647735635636546547676587876857567461213224354565769968274764");
	BigInt::outputBase = 10;
	std::cout << bigval << "\n";

	BigInt bigval2("12341322344444444444444444444441341");
	std::cout << bigval / bigval2 << "\n";
	std::cout << "\n" <<
		bigval *bigval2 << "\n" << 
		-bigval *bigval2 << "\n" <<
		bigval *-bigval2 << "\n" <<
		-bigval *-bigval2 << "\n" <<
		bigval * (BigInt)0 << "\n" <<
		(BigInt)0 *bigval2 << "\n" <<
		(BigInt)0 * 0 << "\n\n" <<
		
		bigval + bigval2 << "\n" <<
		bigval - bigval2 << "\n" << 
		-bigval + bigval2 <<"\n" <<
		-bigval - bigval2 <<"\n\n" <<
		bigval2 + bigval << "\n" <<
		bigval2 - bigval << "\n" <<
		-bigval2 + bigval <<"\n" <<
		-bigval2 - bigval <<"\n";
	system("pause");
}


