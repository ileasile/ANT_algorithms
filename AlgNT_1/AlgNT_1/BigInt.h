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

namespace BigIntUtility {
	constexpr unsigned char _log2(uint64_t n) {
		return ((n <= 2) ? 1 : 1 + _log2(n >> 1));
	}
}

class BigInt {
	
public:
	typedef uint32_t	bui;
	typedef int32_t		bsi;
	typedef uint64_t	lui;
	typedef int64_t		lsi;

	typedef std::pair<BigInt, BigInt> QuRem;
	typedef std::vector<bui> intvec;

	static const unsigned char SOI = sizeof(bui) * 8;
	static const unsigned char SOI_1 = SOI - 1;
	static const lui C_MAX_DIG_1 = (lui)std::numeric_limits<bui>::max() + 1;
	static const lui C_MAX_DIG = (lui)std::numeric_limits<bui>::max();
	static const unsigned char LOG_SOI = BigIntUtility::_log2(SOI);

	template<typename signed_int>
	static char sign(signed_int val);

	template<typename signed_int>
	static signed_int abs_num(signed_int val);

	static unsigned char digval(char digit);

	static std::string dig_by_val(bui val);

	template <typename int_type>
	static std::pair<int_type, int_type> quo_rem(int_type a, int_type b);

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
	const bui & operator[] (size_t i) const {
		return data[i];
	}

	template<unsigned char shift = SOI, lui mask = C_MAX_DIG>
	static inline void shift_con_uu(lui & r, lui & carry);

	template<lui added_val = C_MAX_DIG_1>
	static inline void elementary_subs(lsi & r, lsi & carry);
public:
	BigInt(char sgn, const intvec & data) :sgn(sgn), data(data) {}
	BigInt(char sgn, const BigInt & a) :sgn(sgn), data(a.data) {}
	BigInt(const BigInt & a) : sgn(a.sgn), data(a.data) {}
	BigInt(unsigned long long val, char sign);
	BigInt(signed long long val = 0L) : BigInt(abs_num(val), sign(val)) {}
	BigInt(std::string val, unsigned inB = BigInt::inputBase);

	template <class inttype>
	BigInt(const std::vector<inttype> & v, char sign = 1);

	template<class inttype>
	friend std::vector<inttype> & copy(std::vector<inttype> & v, const BigInt & a);

	~BigInt();

	operator int();
	operator long long();
	operator unsigned();
	operator unsigned long long();

	static unsigned inputBase;
	static BigInt outputBase;
	static bool printPlus;


	friend std::ostream & operator<<(std::ostream & s, const BigInt & a);
	std::string to_string(BigInt base = BigInt::outputBase) const;

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
	BigInt & big_shift(long long n);
	// *this / 2^n
	BigInt operator >> (long long n) const;
	BigInt & operator >>= (long long n);
	// *this * 2^n
	BigInt operator << (long long n) const;
	BigInt & operator <<= (long long n);

	friend BigInt operator * (bui a, const BigInt & b);
	friend BigInt operator * (const BigInt & b, bui a);
	BigInt operator * (const BigInt & a) const;
	BigInt & operator *= (const BigInt & a);
	BigInt & operator *= (const bui a);

	QuRem div(const BigInt & d) const;
	BigInt operator / (const BigInt & d) const;
	BigInt operator % (const BigInt & d) const;
	BigInt & operator/=(const BigInt & a);

	BigInt & operator%=(const BigInt & a);

};