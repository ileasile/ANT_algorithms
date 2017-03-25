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

#ifndef __BIGINT_SIZE_OF_PRIMITIVE
#define __BIGINT_SIZE_OF_PRIMITIVE 32
#endif

#define __BIGINT_PASTER1(x,y) x ## y
#define __BIGINT_EVALUATOR1(x,y)  __BIGINT_PASTER1(x,y)
#define __BIGINT_SIZE_POSTFIX(NAME)  __BIGINT_EVALUATOR1(NAME, __BIGINT_SIZE_OF_PRIMITIVE)

#define __BIGINT_PASTER2(x) template <> struct uint_t< x > { typedef  uint ## x ## _t type;}
#define __BIGINT_EVALUATOR2(x)  __BIGINT_PASTER2(x)
#define __BIGINT_UINT_STRUCT(SIZE)  __BIGINT_EVALUATOR2(SIZE)

static_assert(
	__BIGINT_SIZE_OF_PRIMITIVE == 8 ||
	__BIGINT_SIZE_OF_PRIMITIVE == 16 ||
	__BIGINT_SIZE_OF_PRIMITIVE == 32 ||
	__BIGINT_SIZE_OF_PRIMITIVE == 64, 
	"Size of primitive should be 8, 16, 32 or 64");

namespace BigIntUtility {
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

#if __BIGINT_SIZE_OF_PRIMITIVE == 64
	template <> struct uint_t< 128 > { typedef unsigned __int128 type; }
#endif 

}

class BigInt {
	
public:
	//types used by this class
	typedef BigIntUtility::uint_t<__BIGINT_SIZE_OF_PRIMITIVE>::type bui;
	typedef BigIntUtility::uint_t<__BIGINT_SIZE_OF_PRIMITIVE * 2>::type lui;
	typedef std::make_signed<bui>::type		bsi;
	typedef std::make_signed<lui>::type		lsi;

	//service types
	typedef std::pair<BigInt, BigInt> QuRem;
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
	static const BigInt C_1;

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
	BigInt & normalize();
	//returns pointer to internal vector storage
	buip get_ptr();
	buicp get_ptr() const;

public:
	// a = |a| + |b|
	static BigInt & addAbs(BigInt & a, const BigInt & b, long long bigShiftB = 0);
	// a = |a| - |b|
	static BigInt & subAbs(BigInt & a, const BigInt & b, long long bigShiftB = 0);
private:

#if defined(__MACHINEX86_X64) && __MACHINEX86_X64 == __MACHINE
#define __addcarry(a,b,c,d)  ( __BIGINT_SIZE_POSTFIX(_addcarry_u) ((a),(b),(c),(d)) )
#define __subborrow(a,b,c,d)  ( __BIGINT_SIZE_POSTFIX(_subborrow_u) ((a),(b),(c),(d)) )

#else
	inline static unsigned char _addcarry(unsigned char carry, BigInt::bui a, BigInt::bui b, BigInt::bui * res) {
		b += carry;
		carry = b < carry;
		*res = a + b;
		return carry + (*res < a);
	}
	inline static unsigned char _subborrow(unsigned char borrow, BigInt::bui a, BigInt::bui b, BigInt::bui * res) {
		b += borrow;
		borrow = b < borrow;
		*res = a - b;
		return borrow + (a < b);
	}

#define __addcarry(a,b,c,d)  ( _addcarry((a),(b),(c),(d)) )
#define __subborrow(a,b,c,d)  ( _subborrow((a),(b),(c),(d)) )
#endif

	// a = a + sign * b
	static BigInt & addSign(BigInt & a, const BigInt & b, char sign);

	//pointer versions of multiplication, addAbs, subAbs. MUST HAVE for optimization purposes
	static BigInt mult(buicp a1, buicp a2, buicp b1, buicp b2, char res_sign);
	static BigInt & add_abs_ptr(BigInt & a, buicp b, buicp be, long long bigShiftB = 0);
	static BigInt & sub_abs_ptr(BigInt & a, buicp b, buicp be, long long bigShiftB = 0);

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
	BigInt(char sgn, const BigInt & a) :sgn(sgn), data(a.data) {}
	BigInt(const BigInt & a) : sgn(a.sgn), data(a.data) {}
	BigInt(BigInt && a) : sgn(a.sgn), data(std::move(a.data)) {}
	BigInt(unsigned long long val, char sign);
	BigInt(signed long long val = 0L) : BigInt(abs_num(val), sign(val)) {}
	BigInt(const std::string & val, unsigned inB = BigInt::inputBase);

	//assignment operators
	BigInt & operator=(const BigInt & a);
	BigInt & operator=(BigInt && a);

	//construct by vector of any integer type
	template <class inttype>
	BigInt(const std::vector<inttype> & v, char sign = 1);

	//copy a to v (one to one)
	template<class inttype>
	friend std::vector<inttype> & copy(std::vector<inttype> & v, const BigInt & a);

	//destructor destructs the data vector
	~BigInt();

	//some unary operations and conversions to primitive types
	BigInt operator~ () const;
	bool operator! () const;

	template<typename T>
	explicit operator T ();

	operator bool();
	explicit operator std::string ();

	//static printing options
	static unsigned inputBase;
	static BigInt outputBase;
	static bool printPlus;
	
	//input/output operators/functions
	friend std::ostream & operator<<(std::ostream & s, const BigInt & a);
	friend std::istream & operator>>(std::istream & s, BigInt & a);
	std::string to_string(BigInt base = BigInt::outputBase) const;

	//generate random BigInt of d digits
	static BigInt get_random(unsigned digits);

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
	BigInt & negate();
	//sign of this BigInt
	char signum() const;
	//return the absolute value of this BigInt
	BigInt abs() const;
	//makes this BigInt positive (sets sign = 1)
	BigInt & make_positive();
	//return the number of digits
	size_t dig() const;

	// x <=> y: -1 if x < y else (0 if x = y else 1)
	// |*this| <=> |a|
	char compareAbs(const BigInt & a, long long bigShiftA = 0) const;
	// *this <=> a
	char compare(const BigInt & a) const;
	//comparison operators
	bool operator< (const BigInt & a) const;
	bool operator> (const BigInt & a) const;
	bool operator== (const BigInt & a) const;
	bool operator!= (const BigInt & a) const;
	bool operator<= (const BigInt & a) const;
	bool operator>= (const BigInt & a) const;

	//addition and substraction
	BigInt operator + (const BigInt & a) const;
	BigInt operator - (const BigInt & a) const;
	BigInt operator - () const;
	BigInt & operator += (const BigInt & a);
	BigInt & operator -= (const BigInt & a);
	BigInt & operator ++ ();
	BigInt operator ++ (int);
	BigInt & operator -- ();
	BigInt operator -- (int);

	//shift operators
	// *this * 2^(SOI*n)
	BigInt & big_shift(long long n);
	// *this / 2^n
	BigInt operator >> (long long n) const;
	BigInt & operator >>= (long long n);
	BigInt operator >> (int n) const;
	BigInt & operator >>= (int n);
	// *this * 2^n
	BigInt operator << (long long n) const;
	BigInt & operator <<= (long long n);
	BigInt operator << (int n) const;
	BigInt & operator <<= (int n);

	//multiplication operators
	friend BigInt operator * (bui a, const BigInt & b);
	friend BigInt operator * (const BigInt & b, bui a);
	BigInt operator * (const BigInt & a) const;
	BigInt & operator *= (const BigInt & a);
	BigInt & operator *= (const bui a);

	//division and remainder operators
	void div(const BigInt & d, BigInt & Q, BigInt & R) const;
	static QuRem divmod(const BigInt & a, const BigInt & b);
	BigInt operator / (const BigInt & d) const;
	BigInt operator % (const BigInt & d) const;
	BigInt & operator/=(const BigInt & a);
	BigInt & operator%=(const BigInt & a);

};

template<typename T>
BigInt::operator T () {
	static_assert(sizeof(T) % sizeof(BigInt::bui) == 0 && std::is_integral<T>::value, 
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