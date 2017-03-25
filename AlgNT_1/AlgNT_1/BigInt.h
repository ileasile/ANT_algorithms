#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <limits>
#include <utility>
#include <sstream>
#include <algorithm>
#include <tuple>
#include <chrono>
#include <random>
#include <type_traits>
#include <functional>
#include <intrin.h>
#include "BigIntException.h"

namespace BigIntUtility {

#define __BIGINT_PASTER2(x) template <> struct uint_t< x > { typedef  uint ## x ## _t type;}
#define __BIGINT_EVALUATOR2(x)  __BIGINT_PASTER2(x)
#define __BIGINT_UINT_STRUCT(SIZE)  __BIGINT_EVALUATOR2(SIZE)

#define __BIGINT_PASTER3(FUNNAME, x) template<> inline static unsigned char FUNNAME <uint ## x ##_t>\
			(unsigned char carry, uint ## x ## _t a, uint ## x ## _t b, uint ## x ## _t * res) \
			{ return _ ## FUNNAME ## _u ## x(carry, a, b, res); }
#define __BIGINT_EVALUATOR3(FUNNAME, x)  __BIGINT_PASTER3(FUNNAME, x)
#define __BIGINT_CARRY_FUNCTION(FUNNAME, SIZE)  __BIGINT_EVALUATOR3(FUNNAME, SIZE)

	//constexpr functions to count integer logarithms
	constexpr unsigned char _log2(uint64_t n) {
		return ((n <= 1) ? 0 : 1 + _log2(n >> 1));
	}
	constexpr unsigned char _logb(uint64_t n, uint32_t base) {
		return ((n <= 1) ? 0 : 1 + _logb(n / base, base));
	}

	template <int bits> struct uint_t {};
	__BIGINT_UINT_STRUCT(8);
	__BIGINT_UINT_STRUCT(16);
	__BIGINT_UINT_STRUCT(32);
	__BIGINT_UINT_STRUCT(64);

#if SIZE == 64
	template <> struct uint_t< 128 > { typedef unsigned __int128 type; }
#endif 

#if defined(__MACHINEX86_X64) && __MACHINEX86_X64 == __MACHINE
	template<typename T>
	inline static unsigned char addcarry(unsigned char carry, T a, T b, T * res);
	template<typename T>
	inline static unsigned char subborrow(unsigned char borrow, T a, T b, T * res);

	__BIGINT_CARRY_FUNCTION(addcarry, 8);
	__BIGINT_CARRY_FUNCTION(addcarry, 16);
	__BIGINT_CARRY_FUNCTION(addcarry, 32);
	__BIGINT_CARRY_FUNCTION(addcarry, 64);

	__BIGINT_CARRY_FUNCTION(subborrow, 8);
	__BIGINT_CARRY_FUNCTION(subborrow, 16);
	__BIGINT_CARRY_FUNCTION(subborrow, 32);
	__BIGINT_CARRY_FUNCTION(subborrow, 64);

#else
	template<typename T>
	inline static unsigned char addcarry(unsigned char carry, T a, T b, T * res) {
		b += carry;
		carry = b < carry;
		*res = a + b;
		return carry + (*res < a);
	}
	template<typename T>
	inline static unsigned char subborrow(unsigned char borrow, T a, T b, T * res) {
		b += borrow;
		borrow = b < borrow;
		*res = a - b;
		return borrow + (a < b);
	}
#endif
}

template <int SIZE = 32 >
class BigInt {
	static_assert(
	SIZE == 8 ||SIZE == 16 ||SIZE == 32 ||SIZE == 64, 
	"Size of primitive should be 8, 16, 32 or 64");
public:
	//types used by this class
	typedef typename BigIntUtility::uint_t<SIZE>::type bui;
	typedef typename BigIntUtility::uint_t<SIZE * 2>::type lui;
	typedef typename std::make_signed<bui>::type		bsi;
	typedef typename std::make_signed<lui>::type		lsi;

	//service types
	typedef std::pair<BigInt<SIZE>, BigInt<SIZE>> QuRem;
	typedef std::vector<bui> intvec;
	
	//pointers to array of basic type
	typedef bui *		buip;
	typedef const bui *		buicp;
	
	//size of basic type in bits
	static const unsigned char SOI = sizeof(bui) * 8;
	static const unsigned char SOI_1 = SOI - 1;
	//2^SOI
	static const lui C_MAX_DIG_1 = (lui)std::numeric_limits<bui>::max() + 1;
	//2^SOI - 1
	static const lui C_MAX_DIG = (lui)std::numeric_limits<bui>::max();
	static const unsigned char LOG_SOI = BigIntUtility::_log2(SOI);
	//approximate size of inernal vector on which Karatsuba multiplication is as effective as naive
	static const unsigned int KARATSUBA_LIMIT = 55;

	//constant 1
	static const BigInt<SIZE> C_1;

	//returns the sign of signed number
	template<typename signed_int>
	static char sign(signed_int val);

	//returns the absolute value of signed number
	template<typename signed_int>
	static signed_int abs_num(signed_int val);

	//returns val^n for unsigned number
	template<typename unsigned_int>
	static unsigned_int pow_num(unsigned_int val, char n);

	//returns n^p : n^p <=C_MAX_DIG, n^(p+1) > C_MAX_DIG
	//sets last_p = p
	static bui last_possible_power(bui n, int & last_p);

	//returns value of digit
	static unsigned char digval(char digit);
	//returns digit by value
	static char dig_by_val(bui val);

	//returns mathematically correct values a/b, a%b
	template <typename int_type>
	static std::pair<int_type, int_type> quo_rem(int_type a, int_type b);

private:
	//the only nonstatic members: signum of this BigInt and its digits
	char sgn;
	intvec data;
	
	//normalize
	BigInt<SIZE> & normalize();
	//returns pointer to internal vector storage
	buip get_ptr();
	buicp get_ptr() const;

public:
	// a = |a| + |b|
	static BigInt<SIZE> & addAbs(BigInt<SIZE> & a, const BigInt<SIZE> & b, long long bigShiftB = 0);
	// a = |a| - |b|
	static BigInt<SIZE> & subAbs(BigInt<SIZE> & a, const BigInt<SIZE> & b, long long bigShiftB = 0);
private:
	inline static unsigned char addcarry(unsigned char carry, bui a, bui b, bui * res) {
		return BigIntUtility::addcarry<bui>(carry, a, b, res);
	}
	inline static unsigned char subborrow(unsigned char carry, bui a, bui b, bui * res) {
		return BigIntUtility::subborrow<bui>(carry, a, b, res);
	}

	// a = a + sign * b
	static BigInt<SIZE> & addSign(BigInt<SIZE> & a, const BigInt<SIZE> & b, char sign);

	//pointer versions of multiplication, addAbs, subAbs. MUST HAVE for optimization purposes
	static BigInt<SIZE> mult(buicp a1, buicp a2, buicp b1, buicp b2, char res_sign);
	static BigInt<SIZE> & add_abs_ptr(BigInt<SIZE> & a, buicp b, buicp be, long long bigShiftB = 0);
	static BigInt<SIZE> & sub_abs_ptr(BigInt<SIZE> & a, buicp b, buicp be, long long bigShiftB = 0);

	//indexing operators - return references to elements of data
	inline bui & operator[] (size_t i) {
		return data[i];
	}
	inline const bui & operator[] (size_t i) const {
		return data[i];
	}

public:
	//different constructors
	BigInt(char sgn, const intvec & data) :sgn(sgn), data(data) {}
	BigInt(char sgn, const BigInt<SIZE> & a) :sgn(sgn), data(a.data) {}
	BigInt(const BigInt<SIZE> & a) : sgn(a.sgn), data(a.data) {}
	BigInt(BigInt<SIZE> && a) : sgn(a.sgn), data(std::move(a.data)) {}
	BigInt(unsigned long long val, char sign);
	BigInt(signed long long val = 0L) : BigInt<SIZE>(abs_num(val), sign(val)) {}
	BigInt(const std::string & val, unsigned inB = BigInt<SIZE>::inputBase);

	//assignment operators
	BigInt<SIZE> & operator=(const BigInt<SIZE> & a);
	BigInt<SIZE> & operator=(BigInt<SIZE> && a);

	//construct by vector of any integer type
	template <class inttype>
	BigInt(const std::vector<inttype> & v, char sign = 1);

	//copy a to v (one to one)
	template<class inttype>
	friend std::vector<inttype> & copy(std::vector<inttype> & v, const BigInt<SIZE> & a);

	//destructor destructs the data vector
	~BigInt();

	//some unary operations and conversions to primitive types
	BigInt<SIZE> operator~ () const;
	bool operator! () const;

	template<typename T>
	explicit operator T ();

	operator bool();
	explicit operator std::string ();

	//static printing options
	static unsigned inputBase;
	static BigInt<SIZE> outputBase;
	static bool printPlus;
	
	//input/output operators/functions
	friend std::ostream & operator<<(std::ostream & s, const BigInt<SIZE> & a);
	friend std::istream & operator>>(std::istream & s, BigInt<SIZE> & a) {
		std::string str;
		s >> str;
		a = BigInt<SIZE>(str);
		return s;
	}
	std::string to_string(BigInt<SIZE> base = BigInt<SIZE>::outputBase) const;

	//generate random BigInt of d digits
	static BigInt<SIZE> get_random(unsigned digits);

public:
	//check if this BigInt is null
	bool isNull() const;
	//check if this BigInt is negative
	bool isNeg() const;
	//check if this BigInt is positive
	inline bool isPos() const;
	//check if this BigInt is odd
	bool isOdd() const;
	//check if this BigInt is even
	bool isEven() const;

	//inverts the sign
	BigInt<SIZE> & negate();
	//sign of this BigInt
	char signum() const;
	//return the absolute value of this BigInt
	BigInt<SIZE> abs() const;
	//makes this BigInt positive (sets sign = 1)
	BigInt<SIZE> & make_positive();
	//return the number of digits
	size_t dig() const;

	// x <=> y: -1 if x < y else (0 if x = y else 1)
	// |*this| <=> |a|
	char compareAbs(const BigInt<SIZE> & a, long long bigShiftA = 0) const;
	// *this <=> a
	char compare(const BigInt<SIZE> & a) const;
	//comparison operators
	bool operator< (const BigInt<SIZE> & a) const;
	bool operator> (const BigInt<SIZE> & a) const;
	bool operator== (const BigInt<SIZE> & a) const;
	bool operator!= (const BigInt<SIZE> & a) const;
	bool operator<= (const BigInt<SIZE> & a) const;
	bool operator>= (const BigInt<SIZE> & a) const;

	//addition and substraction
	BigInt<SIZE> operator + (const BigInt<SIZE> & a) const;
	BigInt<SIZE> operator - (const BigInt<SIZE> & a) const;
	BigInt<SIZE> operator - () const;
	BigInt<SIZE> & operator += (const BigInt<SIZE> & a);
	BigInt<SIZE> & operator -= (const BigInt<SIZE> & a);
	BigInt<SIZE> & operator ++ ();
	BigInt<SIZE> operator ++ (int);
	BigInt<SIZE> & operator -- ();
	BigInt<SIZE> operator -- (int);

	//shift operators
	// *this * 2^(SOI*n)
	BigInt<SIZE> & big_shift(long long n);
	// *this / 2^n
	BigInt<SIZE> operator >> (long long n) const;
	BigInt<SIZE> & operator >>= (long long n);
	BigInt<SIZE> operator >> (int n) const;
	BigInt<SIZE> & operator >>= (int n);
	// *this * 2^n
	BigInt<SIZE> operator << (long long n) const;
	BigInt<SIZE> & operator <<= (long long n);
	BigInt<SIZE> operator << (int n) const;
	BigInt<SIZE> & operator <<= (int n);

	//multiplication operators
	friend BigInt<SIZE> operator * (bui a, const BigInt<SIZE> & b);
	friend BigInt<SIZE> operator * (const BigInt<SIZE> & b, bui a);
	BigInt<SIZE> operator * (const BigInt<SIZE> & a) const;
	BigInt<SIZE> & operator *= (const BigInt<SIZE> & a);
	BigInt<SIZE> & operator *= (const bui a);

	//division and remainder operators
	void div(const BigInt<SIZE> & d, BigInt<SIZE> & Q, BigInt<SIZE> & R) const;
	static QuRem divmod(const BigInt<SIZE> & a, const BigInt<SIZE> & b);
	BigInt<SIZE> operator / (const BigInt<SIZE> & d) const;
	BigInt<SIZE> operator % (const BigInt<SIZE> & d) const;
	BigInt<SIZE> & operator/=(const BigInt<SIZE> & a);
	BigInt<SIZE> & operator%=(const BigInt<SIZE> & a);

};

template<int SIZE> 
template <typename T>
BigInt<SIZE> ::operator T () {
	static_assert(sizeof(T) % sizeof(BigInt<SIZE>::bui) == 0 && std::is_integral<T>::value, 
		"BigInt may be converted only to integer type which size is multiplicand of sizeof(bui)");

	const int iters = sizeof(T) / sizeof(bui);
	std::make_unsigned<T>::type res = 0;
	for (size_t i = iters; i --> 0;) {
		if (i < dig()) {
			res <<= SOI;
			res += data[i];
		}
	}
	return T(res) * sgn;
}

template<int SIZE>
unsigned BigInt<SIZE>::inputBase = 10;
template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::outputBase = 10;
template<int SIZE>
bool BigInt<SIZE>::printPlus = false;
template<int SIZE>
const BigInt<SIZE> BigInt<SIZE>::C_1 = BigInt<SIZE>(1, { 1 });

template<int SIZE>
template<typename signed_int>
char BigInt<SIZE>::sign(signed_int val) {
	return (val > 0) ? 1 : ((val == 0) ? 0 : -1);
}

template<int SIZE>
template<typename signed_int>
signed_int BigInt<SIZE>::abs_num(signed_int val) {
	return (val > 0) ? val : -val;
}

template<int SIZE>
template<typename unsigned_int>
unsigned_int BigInt<SIZE>::pow_num(unsigned_int val, char n)
{
	auto res = 1;
	while (n > 0) {
		res *= val;
		--n;
	}
	return res;
}

template<int SIZE>
typename BigInt<SIZE>::bui BigInt<SIZE>::last_possible_power(bui n, int & last_p)
{
	last_p = 0;
	lui last_pow = 1;
	while (1) {
		last_pow *= n;
		if (last_pow > std::numeric_limits<bui>::max()) {
			last_pow /= n;
			return bui(last_pow);
		}
		++last_p;
	}
}
template<int SIZE>
unsigned char BigInt<SIZE>::digval(char digit) {
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

template<int SIZE>
char BigInt<SIZE>::dig_by_val(bui val) {
	if (val >= 0 && val <= 9) {
		return '0' + val;
	}
	if (val >= 10 && val <= 35) {
		return 'A' + val - 10;
	}
	throw BigIntIllegalDigitException();
}

template<int SIZE>
template<typename int_type>
std::pair<int_type, int_type> BigInt<SIZE>::quo_rem(int_type a, int_type b)
{
	///TODO: remake with div()
	int_type q = a / b;
	int_type r = a % b;
	if (q >= 0)
		return std::make_pair(q, r);
	else
		return std::make_pair(q - 1, r + b);
}

template<int SIZE, class inttype>
std::vector<inttype> & copy(std::vector<inttype> & v, const BigInt<SIZE> & a) {
	v.resize(a.dig());
	for (size_t i = 0; i < v.size(); ++i) {
		v[i] = a[i];
	}
	return v;
}

template<int SIZE>
BigInt<SIZE>::BigInt(const std::string & val, unsigned inB) {
	if (val.empty()) {
		sgn = 0;
		return;
	}

	int MAX_CHAR_READ;
	auto last_pow = last_possible_power(inB, MAX_CHAR_READ);

	char res_sgn = 1;
	auto p = val.data();
	auto pEnd = p + val.size();

	if (*p == '-') {
		res_sgn = -1;
		++p;
	}
	else if (*p == '+') {
		++p;
	}

	data.reserve((size_t)((val.size() + 1) / log10l((long double)C_MAX_DIG_1)) + 1);
	unsigned char_read = 0, r = 0;
	for (; p != pEnd; ++p) {
		r *= inB;
		r += digval(*p);
		++char_read;
		if (char_read == MAX_CHAR_READ) {
			*this *= (unsigned)last_pow;
			addAbs(*this, r);
			r = char_read = 0;
		}
	}
	if (char_read != 0) {
		*this *= pow_num(inB, char_read);
		addAbs(*this, r);
	}

	sgn = res_sgn;
	normalize();
}

template<int SIZE>
BigInt<SIZE>::BigInt(unsigned long long val, char sign) {
	sgn = val == 0 ? 0 : sign;

	while (val > 0) {
		data.push_back((bui)(val & C_MAX_DIG));
		val >>= SOI;
	}
}

template<int SIZE>
template<class inttype>
BigInt<SIZE>::BigInt(const std::vector<inttype>& v, char sign) {
	data.resize(v.size());
	auto itv = v.cbegin();
	for (auto it = data.begin(); it != data.end(); ++it, ++itv) {
		*it = (bui)*itv;
	}
	sgn = sign;
	normalize();
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator=(const BigInt<SIZE> & a) {
	if (this != &a) {
		sgn = a.sgn;
		data = a.data;
	}
	return *this;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator=(BigInt<SIZE> && a) {
	if (this != &a) {
		sgn = a.sgn;
		data = std::move(a.data);
	}
	return *this;
}

template<int SIZE>
BigInt<SIZE>::~BigInt() {
	data.clear();
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator~() const {
	return -C_1 - *this;
}

template<int SIZE>
bool BigInt<SIZE>::operator!() const {
	return isNull();
}

template<int SIZE>
BigInt<SIZE>::operator bool() {
	return !isNull();
}

template<int SIZE>
BigInt<SIZE>::operator std::string() {
	return to_string();
}

template<int SIZE>
std::ostream & operator <<(std::ostream & s, const BigInt<SIZE> & a) {
	return s << a.to_string();
}

template<int SIZE>
std::string BigInt<SIZE>::to_string(BigInt<SIZE> base) const {
	std::vector<std::string> out;
	std::ostringstream s;
	if (isNeg() || BigInt<SIZE>::printPlus)
		s << (isNeg() ? '-' : '+');
	if (isNull()) {
		out.push_back("0");
	}
	else {
		BigInt<SIZE> A = abs(), R, Q;

		int char_written;
		bui base_int = base[0];
		BigInt<SIZE> PowerB = last_possible_power(base_int, char_written);
		std::string str(char_written, '0');

		while (A.isPos()) {
			A.div(PowerB, Q, R);
			A = Q;
			bui r0 = bui(R);

			if (A.isNull()) {
				str = "";
				while (r0) {
					str = BigInt<SIZE>::dig_by_val(r0 % base_int) + str;
					r0 /= base_int;
				}
			}
			else {
				std::fill(str.begin(), str.end(), '0');
				int i = char_written;
				while (r0) {
					str[--i] = BigInt<SIZE>::dig_by_val(r0 % base_int);
					r0 /= base_int;
				}
			}

			out.push_back(str);
		}
	}
	for (auto it = out.rbegin(); it != out.rend(); ++it) {
		s << *it;
	}
	return s.str();
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::get_random(unsigned digits) {
	static std::mt19937_64 gen((unsigned)std::chrono::system_clock::now().time_since_epoch().count());

	BigInt<SIZE> res;
	if (digits) {
		res.data.resize(digits);
		size_t k = res.data.size();
		bui g = bui(gen());
		res[--k] = g ? g : g + 1;
		for (; k-- > 0;) {
			res[k] = bui(gen());
		}
		res.sgn = (gen() & 1) ? 1 : -1;
	}

	return res;
}

template<int SIZE>
bool BigInt<SIZE>::isNull() const {
	return sgn == 0;
}
template<int SIZE>
bool BigInt<SIZE>::isNeg() const {
	return sgn == -1;
}
template<int SIZE>
bool BigInt<SIZE>::isPos() const {
	return sgn == 1;
}
template<int SIZE>
bool BigInt<SIZE>::isOdd() const {
	return !isNull() && ((data[0] & 1) != 0);
}
template<int SIZE>
bool BigInt<SIZE>::isEven() const {
	return isNull() || ((data[0] & 1) == 0);
}

template<int SIZE>
char BigInt<SIZE>::signum() const {
	return sgn;
}
template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::abs() const {
	if (isNeg()) return -*this;
	else return *this;
}
template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::make_positive()
{
	if (*this) {
		sgn = 1;
	}
	return *this;
}
template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::negate() {
	sgn = -sgn;
	return *this;
}
template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::normalize() {
	int i = (int)data.size() - 1;
	for (; i >= 0; --i) {
		if (data[i] != 0)
			break;
	}
	++i;
	data.resize(i);
	if (data.size() == 0) sgn = 0;
	return *this;
}
template<int SIZE>
typename BigInt<SIZE>::buip BigInt<SIZE>::get_ptr(){
	return data.data();
}
template<int SIZE>
typename BigInt<SIZE>::buicp BigInt<SIZE>::get_ptr() const
{
	return data.data();
}
template<int SIZE>
size_t BigInt<SIZE>::dig() const {
	return data.size();
}

template<int SIZE>
char BigInt<SIZE>::compareAbs(const BigInt<SIZE> & a, long long bigShiftA) const {
	if (data.size() != a.data.size() + bigShiftA) {
		return data.size() < (a.data.size() + bigShiftA) ? -1 : 1;
	}
	int i = (int)data.size() - 1;
	for (; i >= bigShiftA; --i) {
		if (data[i] != a.data[(size_t)(i - bigShiftA)]) {
			return data[i] < a.data[(size_t)(i - bigShiftA)] ? -1 : 1;
		}
	}
	for (; i >= 0; --i) {
		if (data[i] != 0)
			return 1;
	}
	return 0;
}

template<int SIZE>
char BigInt<SIZE>::compare(const BigInt<SIZE> & a) const {
	if (sgn != a.sgn)
		return sgn < a.sgn ? -1 : 1;
	return ((sgn + a.sgn) >> 1) * compareAbs(a);
}
template<int SIZE>
bool BigInt<SIZE>::operator< (const BigInt<SIZE> & a) const {
	return compare(a) == -1;
}
template<int SIZE>
bool BigInt<SIZE>::operator> (const BigInt<SIZE> & a) const {
	return compare(a) == 1;
}
template<int SIZE>
bool BigInt<SIZE>::operator== (const BigInt<SIZE> & a) const {
	return compare(a) == 0;
}
template<int SIZE>
bool BigInt<SIZE>::operator!= (const BigInt<SIZE> & a) const {
	return compare(a) != 0;
}
template<int SIZE>
bool BigInt<SIZE>::operator<= (const BigInt<SIZE> & a) const {
	return compare(a) <= 0;
}
template<int SIZE>
bool BigInt<SIZE>::operator>= (const BigInt<SIZE> & a) const {
	return compare(a) >= 0;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::addAbs(BigInt<SIZE> & a, const BigInt<SIZE> & b, long long bigShiftB) {
	return add_abs_ptr(a, b.data.data(), b.data.data() + b.data.size(), bigShiftB);
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::subAbs(BigInt<SIZE> & a, const BigInt<SIZE> & b, long long bigShiftB) {
	return sub_abs_ptr(a, b.data.data(), b.data.data() + b.data.size(), bigShiftB);
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::addSign(BigInt<SIZE> & a, const BigInt<SIZE> & b, char sign) {
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

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::mult(buicp a, buicp ae, buicp b, buicp be, char res_sign){
	size_t k = size_t(ae - a), l = size_t(be - b);
	if (!k || !l)
		return BigInt<SIZE>();

	BigInt<SIZE> res;
	auto ke = k + (k & 1), le = l + (l & 1);

	if (std::min(k, l) < KARATSUBA_LIMIT) {
		lui tmp;
		bui carry;
		res.data.resize(k + l, 0);
		for (size_t i = 0; i < k; ++i) {
			carry = 0;
			for (size_t j = 0; j < l; ++j) {
				tmp = (lui)a[i] * b[j] + (lui)res[i + j] + carry;
				carry = (bui)(tmp >> SOI);
				res[i + j] = tmp & C_MAX_DIG;
			}
			res[i + l] = carry;
		}
	}
	else {
		res.data.reserve(k + l);
		if (ke >= 2 * le) {
			for (size_t i = 0; i < k; i += l) {
				addAbs(res, mult(b, be, a + i, a + i + std::min(l, k - i), 1), i);
			}
		}
		else if (le >= 2 * ke) {
			for (size_t i = 0; i < l; i += k) {
				addAbs(res, mult(a, ae, b + i, b + i + std::min(k, l - i), 1), i);
			}
		}
		else {
			auto m = std::max(ke, le);
			auto m2 = m >> 1;

			BigInt<SIZE> A0, B0;
			A0.data.resize(m2);
			B0.data.resize(m2);
			memcpy(A0.data.data(), a, m2 * sizeof(bui));
			memcpy(B0.data.data(), b, m2 * sizeof(bui));
			A0.sgn = B0.sgn = 1;

			res = mult(a, a + m2, b, b + m2, 1);
			BigInt<SIZE> mid = add_abs_ptr(A0, a + m2, ae) * add_abs_ptr(B0, b + m2, be);
			BigInt<SIZE> A1B1 = mult(a + m2, ae, b + m2, be, 1);
			subAbs(subAbs(mid, res), A1B1);

			addAbs(res, mid, m2);
			addAbs(res, A1B1, m);
		}
	}

	res.sgn = res_sign;
	return res;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::add_abs_ptr(BigInt<SIZE> & a, buicp b, buicp be, long long bigShiftB)
{
	if (be - b == 0) {
		if (a.isNeg())
			a.negate();
		return a;
	}
	auto sh = (size_t)std::min(bigShiftB, (long long)(a.data.size()));
	auto ita = a.data.begin() + sh;
	auto de = b + std::min(be - b, a.data.end() - ita);

	unsigned char carry = 0;
	for (; b != de; ++ita, ++b) {
		carry = addcarry(carry, *ita, *b, &(*ita));
	}
	for (; ita != a.data.end() && carry > 0; ++ita) {
		carry = addcarry(carry, *ita, 0, &(*ita));
	}
	for (; b != be; ++b) {
		bui _a;
		carry = addcarry(carry, 0, *b, &_a);
		a.data.push_back(_a);
	}
	if (carry > 0) {
		a.data.push_back((bui)carry);
	}

	a.sgn = 1;

	return a;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::sub_abs_ptr(BigInt<SIZE> & a, buicp b, buicp be, long long bigShiftB){
	auto sh = (size_t)std::min(bigShiftB, (long long)(a.data.size()));
	auto ita = a.data.begin() + sh;
	auto de = b + std::min(be - b, a.data.end() - ita);

	unsigned char carry = 0;
	for (; b != de; ++ita, ++b) {
		carry = subborrow(carry, *ita, *b, &(*ita));
	}

	for (; ita != a.data.end() && carry != 0; ++ita) {
		carry = subborrow(carry, *ita, 0, &(*ita));
	}

	for (; b != be; ++b) {
		bui _a;
		carry = subborrow(carry, 0, *b, &_a);
		a.data.push_back(_a);
	}
	if (carry) {
		auto it = a.data.begin();
		while (it != a.data.end() && *it == 0) ++it;
		if (it != a.data.end()) {
			*it = (bui)(C_MAX_DIG_1 - *it);
			++it;
			for (; it != a.data.end(); ++it) {
				*it = C_MAX_DIG - *it;
			}
		}
		a.sgn = -1;
	}
	else {
		a.sgn = 1;
	}

	a.normalize();
	return a;
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator + (const BigInt<SIZE> & a) const {
	auto r = *this;
	return r += a;
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator - (const BigInt<SIZE> & a) const {
	auto r = *this;
	return r -= a;
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator - () const {
	BigInt<SIZE> a(*this);
	a.negate();
	return a;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator += (const BigInt<SIZE> & a) {
	return addSign(*this, a, 1);
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator -= (const BigInt<SIZE> & a) {
	return addSign(*this, a, -1);
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator ++ () {
	return addSign(*this, 1, 1);
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator ++ (int) {
	auto a = *this;
	++*this;
	return a;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator -- () {
	return addSign(*this, 1, -1);
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator -- (int) {
	auto a = *this;
	--*this;
	return a;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::big_shift(long long n) {
	if (n > 0)
		data.insert(data.begin(), (size_t)n, 0);
	else if (n < 0)
		data.erase(data.begin(), data.begin() + std::min((size_t)(-n), data.size()));
	return *this;
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator >> (long long n) const {
	auto a(*this);
	return a >>= n;
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator << (long long n) const {
	auto a(*this);
	return a <<= n;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator >>= (long long n) {
	auto skip = n >> LOG_SOI;
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
			*(it - 1) |= (lb << soi_k);

		}

	return normalize();
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator <<= (long long n) {
	if (isNull()) return *this;

	auto skip = n >> LOG_SOI;
	int k = n & SOI_1;
	int soi_k = SOI - k;
	bui lowest = (1 << soi_k) - 1;
	bui hb = 0, lb = 0;

	big_shift(skip);
	if (k == 0)
		return *this;
	for (auto it = data.begin() + (int)skip; it != data.end(); ++it) {
		auto val = *it;
		lb = val & lowest;
		*it = hb | (lb << k);
		hb = val >> soi_k;
	}
	if (hb)
		data.push_back(hb);
	return normalize();
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator >> (int n) const {
	return *this >> (long long)n;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator >>= (int n) {
	return *this >>= (long long)n;
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator << (int n) const {
	return *this << (long long)n;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator <<= (int n) {
	return *this <<= (long long)n;
}

template<int SIZE>
BigInt<SIZE> operator * (typename BigInt<SIZE>::bui a, const BigInt<SIZE> & b) {
	BigInt<SIZE> res(b);
	res *= a;
	return res;
}

template<int SIZE>
BigInt<SIZE> operator * (const BigInt<SIZE> & b, typename BigInt<SIZE>::bui a) {
	return a*b;
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator * (const BigInt<SIZE> & a) const {

	return mult(get_ptr(), get_ptr() + dig(), a.get_ptr(), a.get_ptr() + a.dig(), sgn*a.sgn).normalize();
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator *= (const BigInt<SIZE> & a) {
	return *this = *this * a;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator *= (const bui a)
{
	if (a == 0 || isNull())
		return *this = BigInt<SIZE>();

	lui carry = 0, A = (lui) a;

	for (auto & el : data) {
		lui r = carry + el * A;
		carry = r >> BigInt<SIZE>::SOI;
		el = (bui)(r & BigInt<SIZE>::C_MAX_DIG);
	}

	if (carry > 0) {
		data.push_back((bui)carry);
	}
	return *this;
}

template<int SIZE>
void BigInt<SIZE>::div(const BigInt<SIZE> & d, BigInt<SIZE> & Q, BigInt<SIZE> & R) const
{
	if (d.isNull()) {
		throw BigIntDivideByZeroException();
	}
	if (this->isNull()) {
		Q = R = 0;
		return;
	}

	R = this->abs();
	Q = 0;
	auto B = d.abs();

	if (R.compareAbs(B) != -1) {
		int bits_shift = SOI - BigIntUtility::_log2(B.data.back()) - 1;
		R <<= bits_shift;
		B <<= bits_shift;
		lui eldest_dig = B.data.back();

		int k = (int)R.dig(), l = (int)B.dig();
		Q.data.resize(k - l + 1);
		R.data.reserve(k + 1);

		BigInt<SIZE> BS;
		BS.data.reserve(l + 1);

		for (int i = k - l; i >= 0; --i) {
			R.data.resize(i + l + 1, 0);
			lui temp = (((lui)(R[i + l]) << SOI) | R[i + l - 1]) / eldest_dig;
			temp = (temp > C_MAX_DIG ? C_MAX_DIG : temp);
			R.normalize();

			BS.data = B.data;
			BS.sgn = B.sgn;
			BS *= (bui)temp;
			subAbs(R, BS, i);

			while (R.isNeg())
			{
				subAbs(R, B, i);
				R.negate();
				--temp;
			}
			Q[i] = (bui)temp;
		}
		R >>= bits_shift;
		//R.data.shrink_to_fit();
		Q.sgn = 1;
	}

	if (sgn * d.sgn == -1) {
		Q.negate();
		if (!R.isNull()) {
			--Q;
			subAbs(R, d);
			R.negate();
		}
	}

	if (d.isNeg())
		R.negate();

	Q.normalize();
	R.normalize();
}

template<int SIZE>
typename BigInt<SIZE>::QuRem BigInt<SIZE>::divmod(const BigInt<SIZE> & a, const BigInt<SIZE> & b)
{
	BigInt<SIZE> Q, R;
	a.div(b, Q, R);
	return QuRem(Q, R);
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator / (const BigInt<SIZE> & d) const {
	BigInt<SIZE> Q, R;
	div(d, Q, R);
	return Q;
}

template<int SIZE>
BigInt<SIZE> BigInt<SIZE>::operator % (const BigInt<SIZE> & d) const {
	BigInt<SIZE> Q, R;
	div(d, Q, R);
	return R;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator /= (const BigInt<SIZE> & a) {
	return *this = *this / a;
}

template<int SIZE>
BigInt<SIZE> & BigInt<SIZE>::operator %= (const BigInt<SIZE> & a) {
	return *this = *this % a;
}
