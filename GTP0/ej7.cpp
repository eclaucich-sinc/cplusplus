#include <iostream>

using namespace std;


double intpow(double x, int p) {

	if (p == 1) {
		return x;
	}
	
	if (p == 0) {
		return 1;
	}
	
	if (p % 2 == 0) {
		double z = intpow(x, p / 2);
		return z * z;
	} 
	else {
		double z = intpow(x, (p - 1) / 2);
		return z * z * x;
	}
}

int main() {
	int base = 2;
	int exponentePar = 4;
	int exponenteImpar = 5;
	
	cout << "Calculando " << base << "^" << exponentePar << ":" << endl;
	cout << "Resultado: " << intpow(base, exponentePar) << "\n\n";
	
	cout << "Calculando " << base << "^" << exponenteImpar << ":" << endl;
	cout << "Resultado: " << intpow(base, exponenteImpar) << "\n";
	
	return 0;
}
