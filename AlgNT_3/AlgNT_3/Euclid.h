#pragma once
#include <utility>
#include <exception>
#include "../../AlgNT_1/AlgNT_1/BigInt.h"


namespace Euclid {
	class EuclidException : public std::exception{
		const char * what() const noexcept {
			return "Euclid algorithm: Unknown exception";
		}
	};

	enum class GCDCalcMethod {
		CLASSIC, BINARY
	};

	void sort(BigInt & a, BigInt & b);

	BigInt gcd(BigInt a, BigInt b, GCDCalcMethod method = GCDCalcMethod::CLASSIC);
	BigInt & _gcd1(BigInt & a, BigInt & b);
	BigInt & _gcd2(BigInt & a, BigInt & b);

	BigInt gcd_ext(BigInt a, BigInt b, BigInt & u, BigInt & v, GCDCalcMethod method = GCDCalcMethod::CLASSIC);
	void _gcd1_ext(BigInt & a, BigInt & b, BigInt & u, BigInt & v, BigInt & res);
	void _gcd2_ext(BigInt * a, BigInt * b, BigInt * u, BigInt * v, BigInt * res);
	void _gcd2_ext_pre(BigInt & a, BigInt & b, BigInt & u, BigInt & v, BigInt & res);

	BigInt lcm(const BigInt & a, const BigInt & b, GCDCalcMethod method = GCDCalcMethod::CLASSIC);
}