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
#include "BigIntException.h"

namespace BigIntUtility {
	//constexpr functions to count integer logarithms
	constexpr unsigned char _log2(uint64_t n) {
		return ((n <= 1) ? 0 : 1 + _log2(n >> 1));
	}
	constexpr unsigned char _logb(uint64_t n, uint32_t base) {
		return ((n <= 1) ? 0 : 1 + _logb(n / base, base));
	}
}

class BigInt {
	
public:
	//types used by this class
	//basic types
	typedef uint32_t	bui;
	typedef int32_t		bsi;
	//long types
	typedef uint64_t	lui;
	typedef int64_t		lsi;

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

	//inverts the sign
	BigInt & negate();
	//normalize
	BigInt & normalize();
	//returns pointer to internal vector storage
	buip get_ptr();
	buicp get_ptr() const;

	// a = |a| + |b|
	inline static BigInt & addAbs(BigInt & a, const BigInt & b, long long bigShiftB = 0);
	// a = |a| - |b|
	inline static BigInt & subAbs(BigInt & a, const BigInt & b, long long bigShiftB = 0);
	// a = a + sign * b
	static BigInt & addSign(BigInt & a, const BigInt & b, char sign);

	//elementary subs and add operations
	//in case if internistic versions are not defined
#if !defined(__MACHINEX86_X64) || __MACHINEX86_X64 != __MACHINE
	inline static unsigned char _addcarry_u32(unsigned char carry, unsigned a, unsigned b, unsigned * res) {
		b += carry;
		carry = b < carry;
		*res = a + b;
		return carry + (*res < a);
	}
	inline static unsigned char _subborrow_u32(unsigned char borrow, unsigned a, unsigned b, unsigned * res) {
		b += borrow;
		borrow = b < borrow;
		*res = a - b;
		return borrow + (a < b);
	}
#endif


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
	BigInt(unsigned long long val, char sign);
	BigInt(signed long long val = 0L) : BigInt(abs_num(val), sign(val)) {}
	BigInt(const std::string & val, unsigned inB = BigInt::inputBase);

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
	operator bool();
	explicit operator int();
	explicit operator long long();
	explicit operator unsigned();
	explicit operator unsigned long long();

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
	inline bool isNull() const;
	//check if this BigInt is negative
	inline bool isNeg() const;
	//check if this BigInt is positive
	inline bool isPos() const;
	
	//sign of this BigInt
	inline char signum() const;
	//return the absolute value of this BigInt
	inline BigInt abs() const;
	//return the number of digits
	inline size_t dig() const;

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