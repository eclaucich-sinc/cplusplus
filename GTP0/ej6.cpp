#include <iostream>

using namespace std;

// f() global
void f() {
	cout << "Ejecutando la funcion f() global." << endl;
}

// namespace y su propia funcion f()
namespace my_namespace {
	void f() {
		cout << "Ejecutando la funcion f() dentro de my_namespace." << endl;
	}
}

int main() {
	
	cout << "Llamando a las funciones:" << endl;
	f();
	my_namespace::f();	
	
	cout << "\nComparando las direcciones de memoria:" << endl;
	
	cout << "Puntero de f (global):        " << (void*)f << endl;
	
	cout << "Puntero de my_namespace::f:   " << (void*)(my_namespace::f) << endl;
	
	return 0;
}
