#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
	
	string nombreArchivo = "texto.txt"; 
	
	ifstream archivo(nombreArchivo);
	
	if (!archivo.is_open()) {
		cerr << "Error: No se pudo abrir el archivo '" << nombreArchivo << "'." << endl;
		return 1; 
	}
	
	char caracter;
	int contadorCaracteres = 0;
	
	while (archivo.get(caracter)) {
		contadorCaracteres++;
	}
	
	archivo.close();
	
	cout << "El archivo contiene " << contadorCaracteres << " caracteres." << endl;
	
	return 0;
}
