#include <iostream>

using namespace std;

bool is_prime(int n);
int next_prime(int n);

int main() {
	int n = 1, N = 10;
	
	cout << "Primeros " << N << " numeros primos:" << endl;
	
	for (int k = 0; k < N; k++) {
		n = next_prime(n);
		cout << n << endl; 
	}
	
	return 0;
}

bool is_prime(int n) {
	if (n <= 1) {
		return false;
	}
	
	for (int i = 2; i < n; i++) {
		if (n % i == 0) {
			return false;
		}
	}
	
	return true;
}

int next_prime(int n) {
	int candidato = n + 1;
	
	while (!is_prime(candidato)) {
		candidato++;
	}
	
	return candidato;
}
