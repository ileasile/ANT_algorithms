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

BigInt::bui BigInt::last_possible_power(bui n, int & last_p)
{
	last_p = 0;
	unsigned long long last_pow = 1;
	while (1) {
		last_pow *= n;
		if (last_pow > std::numeric_limits<unsigned>::max()) {
			last_pow /= n;
			return bui(last_pow);
		}
		++last_p;
	}
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
char BigInt::dig_by_val(bui val) {
	if (val >= 0 && val <= 9) {
		return '0' + val;
	}
	if (val >= 10 && val <= 35) {
		return 'A' + val - 10;
	}
	throw BigIntIllegalDigitException();
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
BigInt::BigInt(const std::string & val, unsigned inB) {
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

	data.reserve( (size_t)((val.size() + 1) / log10l((long double)C_MAX_DIG_1)) + 1 );
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

BigInt BigInt::operator~() const{
	return - C_1 - *this ;
}
bool BigInt::operator!() const{
	return isNull();
}
BigInt::operator bool(){
	return !isNull();
}
BigInt::operator int() {
	const int iters = sizeof(int) / sizeof(bui);
	unsigned res = 0;
	for (int i = iters - 1; i >= 0; --i) {
		if ((unsigned)i >= dig())
			continue;
		res <<= SOI;
		res += data[i];
	}
	return (int)res * sgn;
}
BigInt::operator long long() {
	const int iters = sizeof(long long) / sizeof(bui);
	unsigned long long res = 0;
	for (int i = iters - 1; i >= 0; --i) {
		if ((unsigned)i >= dig())
			continue;
		res <<= SOI;
		res += data[i];
	}
	return (long long)res * sgn;
}
BigInt::operator unsigned() {
	const int iters = sizeof(unsigned) / sizeof(bui);
	unsigned res = 0;
	for (int i = iters - 1; i >= 0; --i) {
		if ((unsigned)i >= dig())
			continue;
		res <<= SOI;
		res += data[i];
	}
	return res;
}
BigInt::operator unsigned long long() {
	const int iters = sizeof(unsigned long long) / sizeof(bui);
	unsigned long long res = 0;
	for (int i = iters-1; i >= 0; --i) {
		if ((unsigned)i >= dig())
			continue;
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
		BigInt A = abs(), R, Q;

		int char_written;
		bui base_int = base[0];
		BigInt PowerB = last_possible_power(base_int, char_written);
		std::string str(char_written, '0');

		while (A.isPos()) {
			A.div(PowerB, Q, R);
			A = Q;
			bui r0 = bui(R);

			if (A.isNull()) {
				str = "";
				while (r0) {
					str = BigInt::dig_by_val(r0 % base_int) + str;
					r0 /= base_int;
				}
			}
			else {
				std::fill(str.begin(), str.end(), '0');
				int i = char_written;
				while (r0) {
					str[--i] = BigInt::dig_by_val(r0 % base_int);
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

BigInt BigInt::get_random(unsigned digits){
	static std::mt19937 gen((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	
	BigInt res;
	if (digits) {
		res.data.resize(digits);
		size_t k = res.data.size();
		auto g = gen();
		res[--k] = g ? g : g + 1;
		for (; k-- > 0;) {
			res[k] = gen();
		}
		res.sgn = (gen() & 1) ? 1 : -1;
	}

	return res;
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
bool BigInt::isOdd() const {
	return !isNull() && ((data[0] & 1) != 0);
}
bool BigInt::isEven() const {
	return isNull() || ((data[0] & 1) == 0);
}

char BigInt::signum() const{
	return sgn;
}
BigInt BigInt::abs() const {
	if (isNeg()) return -*this;
	else return *this;
}
BigInt & BigInt::make_positive()
{
	if (*this) {
		sgn = 1;
	}
	return *this;
}
BigInt & BigInt::negate() {
	sgn = -sgn;
	return *this;
}
BigInt & BigInt::normalize() {
	int i = (int)data.size() - 1;
	for (; i >= 0; --i) {
		if (data[i] != 0)
			break;
	}
	data.resize(i + 1);
	if (data.size() == 0) sgn = 0;
	return *this;
}
BigInt::buip BigInt::get_ptr()
{
	return data.data();
}
BigInt::buicp BigInt::get_ptr() const
{
	return data.data();
}
size_t BigInt::dig() const {
	return data.size();
}

char BigInt::compareAbs(const BigInt & a, long long bigShiftA) const {
	if (data.size() != a.data.size() + bigShiftA) {
		return data.size() < (a.data.size() + bigShiftA) ? -1 : 1;
	}
	int i = (int)data.size() - 1;
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

BigInt & BigInt::addAbs(BigInt & a, const BigInt & b, long long bigShiftB) {
	return add_abs_ptr(a, b.data.data(), b.data.data() + b.data.size(), bigShiftB);
}
BigInt & BigInt::subAbs(BigInt & a, const BigInt & b, long long bigShiftB) {
	return sub_abs_ptr(a, b.data.data(), b.data.data() + b.data.size(), bigShiftB);
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

BigInt BigInt::mult(buicp a, buicp ae, buicp b, buicp be, char res_sign)
{
	size_t k = size_t(ae - a), l = size_t(be - b);
	if (!k || !l)
		return BigInt();

	BigInt res;
	auto ke = k + (k & 1), le = l + (l & 1);

	if (std::min(k, l) < KARATSUBA_LIMIT) {
		lui tmp;
		bui carry;
		res.data.resize(k + l, 0);
		for (size_t i = 0; i < k; ++i) {
			carry = 0;
			for (size_t j = 0;j < l;++j) {
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

			BigInt A0, B0;
			A0.data.resize(m2);
			B0.data.resize(m2);
			memcpy(A0.data.data(), a,		m2 * sizeof(bui));
			memcpy(B0.data.data(), b,		m2 * sizeof(bui));
			A0.sgn = B0.sgn = 1;
			
			res = mult(a, a+m2, b, b+m2, 1);
			BigInt mid = add_abs_ptr(A0, a + m2, ae) * add_abs_ptr(B0, b + m2, be);
			BigInt A1B1 = mult(a + m2, ae, b + m2, be, 1);
			subAbs(subAbs(mid, res), A1B1);

			addAbs(res, mid, m2);
			addAbs(res, A1B1, m);
		}
	}

	res.sgn = res_sign;
	return res;
}
BigInt & BigInt::add_abs_ptr(BigInt & a, buicp b, buicp be, long long bigShiftB)
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
	for (;b != de; ++ita, ++b) {
		carry = _addcarry_u32(carry, *ita, *b, &(*ita));
	}
	for (;ita != a.data.end() && carry > 0; ++ita) {
		carry = _addcarry_u32(carry, *ita, 0, &(*ita));
	}
	for (;b != be; ++b) {
		bui _a;
		carry = _addcarry_u32(carry, 0, *b, &_a);
		a.data.push_back(_a);
	}
	if (carry > 0) {
		a.data.push_back((bui)carry);
	}

	a.sgn = 1;

	return a;
}
BigInt & BigInt::sub_abs_ptr(BigInt & a, buicp b, buicp be, long long bigShiftB)
{
	auto sh = (size_t)std::min(bigShiftB, (long long)(a.data.size()));
	auto ita = a.data.begin() + sh;
	auto de = b + std::min(be - b, a.data.end() - ita);

	unsigned char carry = 0;
	for (;b != de; ++ita, ++b) {
		carry = _subborrow_u32(carry, *ita, *b, &(*ita));
	}

	for (;ita != a.data.end() && carry != 0; ++ita) {
		carry = _subborrow_u32(carry, *ita, 0, &(*ita));	
	}

	for (;b != be; ++b) {
		bui _a;
		carry = _subborrow_u32(carry, 0, *b, &_a);
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
	if (n > 0)
		data.insert(data.begin(), (size_t)n, 0);
	else if (n < 0)
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

BigInt BigInt::operator >> (int n) const {
	return *this >> (long long)n;
}
BigInt & BigInt::operator >>= (int n) {
	return *this >>= (long long)n;
}
BigInt BigInt::operator << (int n) const {
	return *this << (long long)n;
}
BigInt & BigInt::operator <<= (int n) {
	return *this <<= (long long)n;
}

BigInt operator * (BigInt::bui a, const BigInt & b) {
	BigInt res(b);
	res *= a;
	return res;
}
BigInt operator * (const BigInt & b, BigInt::bui a) {
	return a*b;
}
BigInt BigInt::operator * (const BigInt & a) const {
	
	return mult(get_ptr(), get_ptr() + dig(), a.get_ptr(), a.get_ptr() + a.dig(), sgn*a.sgn).normalize();
}
BigInt & BigInt::operator *= (const BigInt & a) {
	return *this = *this * a;
}
BigInt & BigInt::operator *= (const bui a)
{
	if (a == 0 || isNull())
		return *this = BigInt();

	BigInt::lui carry = 0, A = (BigInt::lui) a;

	for (auto & el : data) {
		BigInt::lui r = carry + el * A;
		carry = r >> BigInt::SOI;
		el = (BigInt::bui)(r & BigInt::C_MAX_DIG);
	}

	if (carry > 0) {
		data.push_back((BigInt::bui)carry);
	}
	return *this;
}

void BigInt::div(const BigInt & d, BigInt & Q, BigInt & R) const
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
		
		BigInt BS;
		BS.data.reserve(l + 1);

		for (int i = k - l; i >= 0; --i) {
			R.data.resize(i + l + 1, 0);
			lui temp = ( ((lui)(R[i + l]) << SOI) | R[i + l - 1]) / eldest_dig;
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
		--Q;
		subAbs(R, d);
		R.negate();
	}

	if (d.isNeg())
		R.negate();

	Q.normalize();
	R.normalize();
}
BigInt::QuRem BigInt::divmod(const BigInt & a, const BigInt & b)
{
	BigInt Q, R;
	a.div(b, Q, R);
	return QuRem(Q, R);
}
BigInt BigInt::operator / (const BigInt & d) const {
	BigInt Q, R;
	div(d, Q, R);
	return Q;
}
BigInt BigInt::operator % (const BigInt & d) const {
	BigInt Q, R;
	div(d, Q, R);
	return R;
}
BigInt & BigInt::operator /= (const BigInt & a) {
	return *this = *this / a;
}
BigInt & BigInt::operator %= (const BigInt & a) {
	return *this = *this % a;
}