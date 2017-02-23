#include "BigInt.h"

unsigned BigInt::inputBase = 10;
BigInt BigInt::outputBase = 10;
bool BigInt::printPlus = false;
const BigInt BigInt::C_1 = BigInt(1, { 1 });

template<typename signed_int>
char BigInt::sign(signed_int val) {
	return (val > 0) ? 1 : ((val == 0) ? 0 : -1);
}

template<typename signed_int>
signed_int BigInt::abs_num(signed_int val) {
	return (val > 0) ? val : -val;
}

template<typename unsigned_int>
unsigned_int BigInt::pow_num(unsigned_int val, char n)
{
	auto res = 1;
	while (n > 0) {
		res *= val;
		--n;
	}
	return res;
}

unsigned char BigInt::digval(char digit) {
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
std::string BigInt::dig_by_val(bui val) {
	if (val >= 0 && val <= 9) {
		return std::string(1, '0' + val);
	}
	if (val >= 10 && val <= 35) {
		return std::string(1, 'A' + val - 10);
	}
	return "(" + std::to_string(val) + ")";
}

template<typename int_type>
std::pair<int_type, int_type> BigInt::quo_rem(int_type a, int_type b)
{
	///TODO: remake with div()
	int_type q = a / b;
	int_type r = a % b;
	if (q >= 0)
		return std::make_pair(q, r);
	else
		return std::make_pair(q - 1, r + b);
}

template<class inttype>
std::vector<inttype> & copy(std::vector<inttype> & v, const BigInt & a) {
	v.resize(a.dig());
	for (size_t i = 0; i < v.size(); ++i) {
		v[i] = a[i];
	}
	return v;
}
BigInt::BigInt(std::string val, unsigned inB) {
	if (val.empty()) {
		sgn = 0;
		return;
	}

	int MAX_CHAR_READ = 0;
	unsigned long long last_pow = 1;
	while (1) {
		last_pow *= inB;
		if (last_pow > std::numeric_limits<unsigned>::max()) {
			last_pow /=inB;
			break;
		}
		++MAX_CHAR_READ;
	}

	char res_sgn = 1;
	auto it = val.begin();
	if (*it == '-') {
		res_sgn = -1;
		++it;
	}
	else if (*it == '+') {
		++it;
	}

	unsigned char_read = 0, r = 0;
	for (; it != val.end(); ++it) {
		r *= inB;
		r += digval(*it);
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
	/*if (val.empty()) {
		sgn = 0;
		return;
	}

	char sgn = 1;
	auto it = val.begin();
	if (*it == '-') {
		sgn = -1;
		++it;
	}
	else if (*it == '+') {
		++it;
	}

	for (;it != val.end(); ++it) {
		*this *= inB;
		addAbs(*this, digval(*it));
	}
	this->sgn = sgn;
	normalize();*/
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
	sgn = sign;
	normalize();
}

BigInt::~BigInt() {
	data.clear();
}

BigInt::operator int() {
	const int iters = sizeof(int) / sizeof(bui);
	unsigned res = 0;
	for (size_t i = 0; i < iters && i < data.size(); ++i) {
		res <<= SOI;
		res += data[i];
	}
	return (int)res * sgn;
}
BigInt::operator long long() {
	const int iters = sizeof(long long) / sizeof(bui);
	unsigned long long res = 0;
	for (size_t i = 0; i < iters && i < data.size(); ++i) {
		res <<= SOI;
		res += data[i];
	}
	return (long long)res * sgn;
}
BigInt::operator unsigned() {
	const int iters = sizeof(int) / sizeof(bui);
	unsigned res = 0;
	for (size_t i = 0; i < iters && i < data.size(); ++i) {
		res <<= SOI;
		res += data[i];
	}
	return res;
}
BigInt::operator unsigned long long() {
	const int iters = sizeof(long long) / sizeof(bui);
	unsigned long long res = 0;
	for (size_t i = 0; i < iters && i < data.size(); ++i) {
		res <<= SOI;
		res += data[i];
	}
	return res;
}

std::ostream & operator <<(std::ostream & s, const BigInt & a) {
	return s << a.to_string();
}
std::istream & operator >> (std::istream & s, BigInt & a) {
	std::string str;
	s >> str;
	a = BigInt(str);
	return s;
}
std::string BigInt::to_string(BigInt base) const {
	std::vector<std::string> out;
	std::ostringstream s;
	if (isNeg() || BigInt::printPlus)
		s << (isNeg() ? '-' : '+');
	if (isNull()) {
		out.push_back("0");
	}
	else {
		BigInt A = abs(), R;
		while (A.isPos()) {
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

char BigInt::compareAbs(const BigInt & a, long long bigShiftA) const {
	if (data.size() != a.data.size() + bigShiftA) {
		return data.size() < (a.data.size() + bigShiftA) ? -1 : 1;
	}
	int i = data.size() - 1;
	for (;i >= bigShiftA; --i) {
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
char BigInt::compare(const BigInt & a) const {
	if (sgn != a.sgn)
		return sgn < a.sgn ? -1 : 1;
	return ((sgn + a.sgn) >> 1) * compareAbs(a);
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

template<unsigned char shift, BigInt::lui mask>
void BigInt::shift_con_uu(lui & r, lui & carry) {
	carry = r >> shift;
	r &= mask;
}

template<BigInt::lui added_val>
void BigInt::elementary_subs(lsi & r, lsi & carry) {
	if (r < 0) {
		r += added_val;
		carry = -1;
	}
	else {
		carry = 0;
	}
}

BigInt & BigInt::addAbs(BigInt & a, const BigInt & b, long long bigShiftB) {
	if (b.isNull())
		return a;

	auto it1 = a.data.begin() + (size_t)std::min(bigShiftB, (long long) (a.data.size()) );
	auto it2 = b.data.cbegin();

	lui carry = 0;
	for (;it1 != a.data.end() && it2 != b.data.end(); ++it1, ++it2) {
		lui r = carry + *it1 + *it2;
		shift_con_uu(r, carry);
		*it1 = (bui)r;
	}
	for (;it1 != a.data.end(); ++it1) {
		lui r = carry + *it1;
		shift_con_uu(r, carry);
		*it1 = (bui)r;
	}
	for (;it2 != b.data.end(); ++it2) {
		lui r = carry + *it2;
		shift_con_uu(r, carry);
		a.data.push_back((bui)r);
	}
	if (carry > 0) {
		a.data.push_back((bui)carry);
	}

	a.sgn = 1;

	return a;
}
BigInt & BigInt::subAbs(BigInt & a, const BigInt & b, long long bigShiftB) {
	char sgn = a.compareAbs(b, bigShiftB);
	const auto * pa = &a;
	const auto * pb = &b;
	if (sgn == -1) {
		pa = &b;
		pb = &a;
	}
	auto & ra = pa -> data;
	auto & rb = pb -> data;

	auto sh = (size_t)std::min(bigShiftB, (long long)(a.data.size()));
	auto ita = a.data.begin() + sh;
	auto it1 = ra.cbegin() ;
	auto it2 = rb.cbegin();

	if (sgn == -1)
		it2 += sh;
	else
		it1 += sh;

	lsi carry = 0;
	for (;it1 != ra.end() && it2 != rb.end(); ++it1, ++it2, ++ita) {
		lsi r = carry + *it1 - *it2;
		elementary_subs(r, carry);
		*ita = (bui)r;
	}

	if (ita != a.data.end()) {
		for (;ita != a.data.end(); ++ita) {
			lsi r = carry + *ita;
			elementary_subs(r, carry);
			*ita = (bui)r;
		}
	}
	else {
		for (;it1 != ra.end(); ++it1) {
			lsi r = carry + *it1;
			elementary_subs(r, carry);
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

BigInt & BigInt::big_shift(long long n) {
	if (n >= 0)
		data.insert(data.begin(), (size_t)n, 0);
	else
		data.erase(data.begin(), data.begin() +  std::min((size_t)(-n), data.size()) );
	return *this;
}
BigInt BigInt::operator >> (long long n) const {
	auto a(*this);
	return a >>= n;
}
BigInt BigInt::operator << (long long n) const {
	auto a(*this);
	return a <<= n;
}
BigInt & BigInt::operator >>= (long long n) {
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
BigInt & BigInt::operator <<= (long long n) {
	if (isNull()) return *this;

	auto skip = n >> LOG_SOI;
	int k = n & SOI_1;
	int soi_k = SOI - k;
	bui lowest = (1 << soi_k) - 1;
	bui hb = 0, lb = 0;

	big_shift(skip);
	if (k == 0)
		return *this;
	for (auto it = data.begin() + (int)skip;it != data.end();++it) {
		auto val = *it;
		lb = val & lowest;
		*it = hb | (lb << k);
		hb = val >> soi_k;
	}
	if (hb)
		data.push_back(hb);
	return normalize();
}

BigInt operator * (BigInt::bui a, const BigInt & b) {
	if (a == 0 || b.isNull())
		return BigInt();
	
	auto it = b.data.begin();

	BigInt::lui carry = 0;
	BigInt::intvec newdata;
	newdata.reserve(b.dig() + 1);
	for (;it != b.data.end(); ++it) {
		BigInt::lui r = carry + *it * (BigInt::lui)a;
		carry = r >> BigInt::SOI;
		newdata.push_back((BigInt::bui)(r & BigInt::C_MAX_DIG));
	}

	if (carry > 0) {
		newdata.push_back((BigInt::bui)carry);
	}
	return BigInt(b.sgn, newdata);
}
BigInt operator * (const BigInt & b, BigInt::bui a) {
	return a*b;
}
BigInt BigInt::operator * (const BigInt & a) const {
	BigInt res;
	if (isNull() || a.isNull())
		return BigInt();
	res.data.reserve(dig()*a.dig());
	for (size_t i = 0; i < data.size(); ++i) {
		addAbs(res, data[i] * a, i);
	}
	res.sgn = sgn * a.sgn;
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
	if (d.isNull()) {
		throw BigIntDivideByZeroException();
	}
	if (this->isNull()) {
		return (QuRem)std::make_pair(0, 0);
	}
		
	auto R = this->abs(), B = d.abs();
	BigInt Q;

	if (R.compareAbs(B) != -1) {
		int bits_shift = SOI - BigIntUtility::_log2(B.data.back()) - 1;
		R <<= bits_shift;
		B <<= bits_shift;
		lui eldest_dig = B.data.back();

		auto k = R.dig(), l = B.dig();
		Q.data.resize(k - l + 1);
		R.data.reserve(k + 1);

		for (int i = k - l; i >= 0; --i) {
			R.data.resize(i + l + 1, 0);
			lui temp = ( ((lui)(R[i + l]) << SOI) | R[i + l - 1]) / eldest_dig;
			Q[i] = (bui)(temp > C_MAX_DIG ? C_MAX_DIG : temp);
			R.normalize();
			subAbs(R, Q[i] * B, i);
			while (R.isNeg())
			{
				subAbs(R, B, i);
				R.negate();
				--Q[i];
			}
		}
		R >>= bits_shift;
		R.data.shrink_to_fit();
		Q.sgn = 1;
	}

	if (sgn * d.sgn == -1) {
		Q.negate();
		--Q;
		subAbs(R, d);
		R.negate();
	}

	if (d.isNeg())
		R.negate();

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
BigInt & BigInt::operator %= (const BigInt & a) {
	return *this = *this % a;
}