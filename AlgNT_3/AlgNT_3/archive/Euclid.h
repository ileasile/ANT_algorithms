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

	typedef BigInt_t<> BI;

	void sort(BI & a, BI & b);

	BI gcd(BI a, BI b, GCDCalcMethod method = GCDCalcMethod::CLASSIC);
	BI & _gcd1(BI & a, BI & b);
	BI & _gcd2(BI & a, BI & b);

	BI gcd_ext(BI a, BI b, BI & u, BI & v, GCDCalcMethod method = GCDCalcMethod::CLASSIC);
	void _gcd1_ext(BI & a, BI & b, BI & u, BI & v, BI & res);
	void _gcd2_ext(BI & a, BI & b, BI & u, BI & v, BI & res);
	void _gcd2_ext_pre(BI & a, BI & b, BI & u, BI & v, BI & res);

	BI lcm(const BI & a, const BI & b, GCDCalcMethod method = GCDCalcMethod::CLASSIC);
}