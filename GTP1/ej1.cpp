#include <iostream>

using namespace std;

int NCALLS = 0;

double intpow(double x, int p, bool vrbs=false) {

    NCALLS += 1;
    if (vrbs) {
        cout << "Llamada número " << NCALLS << endl;
    }

	if (p == 1) {
		return x;
	}
	
	if (p == 0) {
		return 1;
	}
	
	if (p % 2 == 0) {
		double z = intpow(x, p / 2, vrbs);
		return z * z;
	} 
	else {
		double z = intpow(x, (p - 1) / 2, vrbs);
		return z * z * x;
	}
}

int main() {
	int base = 1;
	
    NCALLS = 0;
    int exponente = 100;
	cout << "Calculando " << base << "^" << exponente << ":" << endl;
	cout << "Resultado: " << intpow(base, exponente, true) << "\n\n";

    NCALLS = 0;
    exponente = 1000;
	cout << "Calculando " << base << "^" << exponente << ":" << endl;
	cout << "Resultado: " << intpow(base, exponente, true) << "\n\n";

    NCALLS = 0;
    exponente = 10000;
	cout << "Calculando " << base << "^" << exponente << ":" << endl;
	cout << "Resultado: " << intpow(base, exponente, true) << "\n\n";
	
	return 0;
}
