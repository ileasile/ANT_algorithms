#include "Euclid.h"

void Euclid::sort(BigInt & a, BigInt & b) {
	if (a.compareAbs(b) == -1)
		std::swap(a, b);
}

BigInt Euclid::gcd(BigInt a, BigInt  b, GCDCalcMethod method) {
	switch (method)
	{
	case GCDCalcMethod::CLASSIC:
		return _gcd1(a, b);
	case GCDCalcMethod::BINARY:
		return _gcd2(a, b);
	default:
		throw EuclidException();
	}

}

BigInt & Euclid::_gcd1(BigInt & a, BigInt & b) {
	BigInt r, d;
	while (b) {
		a.div(b, d, r);
		a = b;
		b = r;
	}
	
	return a.make_positive();
}

BigInt & Euclid::_gcd2(BigInt & a, BigInt & b) {
	if (b.isNull())
		return a.make_positive();
	if (a.isNull())
		return b.make_positive();

	long long sh = 0;
	for (; a.isEven() && b.isEven(); ++sh) {
		a >>= 1;
		b >>= 1;
	}

	while (a.isEven())
		a >>= 1;

	do{
		while (b.isEven())
			b >>= 1;
		sort(b, a);
		BigInt::subAbs(b, a);
	} while (b);

	a <<= sh;

	return a.make_positive();
}

BigInt Euclid::gcd_ext(BigInt a, BigInt b, BigInt & u, BigInt & v, GCDCalcMethod method)
{
	BigInt res;
	char as, bs;
	switch (method)
	{
	case GCDCalcMethod::CLASSIC:
		_gcd1_ext(a, b, u, v, res);
		if (res.isNeg()) {
			res.negate();
			u.negate();
			v.negate();
		}
		break;
	case GCDCalcMethod::BINARY:
		as = a.signum(), bs = b.signum();
		_gcd2_ext_pre(a.make_positive(), b.make_positive(), u, v, res);
		if (as == -1) u.negate();
		if (bs == -1) v.negate();
		break;
	default:
		throw EuclidException();
	}
	
	return res;
}

void Euclid::_gcd1_ext(BigInt & a, BigInt & b, BigInt & u, BigInt & v, BigInt & res){
	BigInt r, q, u0, v0, ut, vt;
	v = u0 = 0;
	u = v0 = 1;
	

	while (b) {
		a.div(b, q, r);
		a = b;
		b = r;

		ut = u0;
		u0 = u - u0*q;
		u = ut;

		vt = v0;
		v0 = v - v0*q;
		v = vt;
	}
	res = a;
	return;
}

void Euclid::_gcd2_ext(BigInt & a, BigInt & b, BigInt & u, BigInt & v, BigInt & res){
	//a is even, b is odd
	if (a.isEven()) {
		a >>= 1;
		BigInt _a = a, _b = b;

		_gcd2_ext(a, b, u, v, res);
		if (u.isEven()) {
			u >>= 1;
		}
		else {
			if (u.isNeg()) {
				u >>= 1; 
				--u;
			}
			else {
				u >>= 1;
			}
			_b >>= 1;
			u -= _b;
			v += _a;
		}
	}

	//a is odd, b is even
	else if (b.isEven()) {
		_gcd2_ext(b, a, v, u, res);
	}

	else {
		auto cmp = a.compareAbs(b);
		// a = b
		if (cmp == 0) {
			u = 1;
			v = 0;
			res = a;
		}

		//a and b are odd, a>b
		else if (cmp > 0) {
			BigInt _b = b;
			_gcd2_ext((a - b) >> 1, b, u, v, res);
			if (u.isEven()) {
				u >>= 1;
				v -= u;
			}
			else {
				v -= (u - a) >> 1;
				u -= _b;
				u >>= 1;
			}
		}

		//a and b are odd, a<b
		else {
			_gcd2_ext(b, a, v, u, res);
		}
	}
}

void Euclid::_gcd2_ext_pre(BigInt & a, BigInt & b, BigInt & u, BigInt & v, BigInt & res){
	if (b.isNull()) {
		v = 0;
		u = 1;
		res = a;
		return;
	}
	if (a.isNull()) {
		u = 0;
		v = 1;
		res = b;
		return;
	}
		
	long long sh = 0;
	for (; a.isEven() && b.isEven(); ++sh) {
		a >>= 1;
		b >>= 1;
	}
	_gcd2_ext(a, b, u, v, res);
	res <<= sh;
}

BigInt Euclid::lcm(const BigInt & a, const BigInt & b, GCDCalcMethod method)
{
	return (a.isNull() && b.isNull()) ? 0 :((a / gcd(a, b, method)) * b).abs();
}
