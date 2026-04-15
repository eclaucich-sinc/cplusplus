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
	
	string palabra;
	int contadorPalabras = 0;
	
	while (archivo >> palabra) {
		contadorPalabras++;
	}
	
	archivo.close();
	
	cout << "El archivo contiene " << contadorPalabras << " palabras." << endl;
	
	return 0;
}
