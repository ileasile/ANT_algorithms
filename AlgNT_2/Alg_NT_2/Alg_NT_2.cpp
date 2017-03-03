#include <mpirxx.h>
#include <iostream>

int main(){
	mpz_class m1("9999999999999899879999999999999999999999999999"), m2(1);
	mpz_class mr = m1 + m2;
	std::cout << mr << std::endl;

	system("pause");
	return 0;
}
