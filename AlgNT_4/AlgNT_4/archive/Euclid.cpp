#include "Euclid.h"

void Euclid::sort(BI & a, BI & b) {
	if (a.compareAbs(b) == -1)
		std::swap(a, b);
}

Euclid::BI Euclid::gcd(BI a, BI  b, GCDCalcMethod method) {
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

Euclid::BI & Euclid::_gcd1(BI & a, BI & b) {
	BI r, d;
	while (b) {
		a.div(b, d, r);
		a = b;
		b = r;
	}
	
	return a.make_positive();
}

Euclid::BI & Euclid::_gcd2(BI & a, BI & b) {
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
		BI::subAbs(b, a);
	} while (b);

	a <<= sh;

	return a.make_positive();
}

Euclid::BI Euclid::gcd_ext(BI a, BI b, BI & u, BI & v, GCDCalcMethod method)
{
	BI res;
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

void Euclid::_gcd1_ext(BI & a, BI & b, BI & u, BI & v, BI & res){
	BI r, q, u0, v0, ut, vt;
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

void Euclid::_gcd2_ext(BI & a, BI & b, BI & u, BI & v, BI & res) {
	//a is even, b is odd
	if (a.isEven()) {
		a >>= 1;
		BI _a = a, _b = b;

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
			BI _b = b, _a = a;
			BI::subAbs(a, b) >>= 1;
			_gcd2_ext(a, b, u, v, res);
			if (u.isEven()) {
				u >>= 1;
				v -= u;
			}
			else {
				v -= (u - _a) >> 1;
				u -= _b;
				u >>= 1;
			}
		}

		//a and b are odd, a<b
		else {
			BI _b = b, _a = a;
			BI::subAbs(b, a) >>= 1;
			_gcd2_ext(b, a, v, u, res);
			if (v.isEven()) {
				v >>= 1;
				u -= v;
			}
			else {
				u -= (v - _b) >> 1;
				v -= _a;
				v >>= 1;
			}
		}
	}
}

void Euclid::_gcd2_ext_pre(BI & a, BI & b, BI & u, BI & v, BI & res){
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

	if (b.isEven()) {
		_gcd2_ext(b, a, v, u, res);
	}
	else {
		_gcd2_ext(a, b, u, v, res);
	}
	res <<= sh;
}

Euclid::BI Euclid::lcm(const BI & a, const BI & b, GCDCalcMethod method)
{
	return (a.isNull() && b.isNull()) ? 0 :((a / gcd(a, b, method)) * b).abs();
}
