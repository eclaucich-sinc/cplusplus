#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main() {
	
	string nombreArchivo = "texto.txt"; 
	
	ifstream archivo(nombreArchivo);
	
	if (!archivo.is_open()) {
		cerr << "Error: No se pudo abrir el archivo '" << nombreArchivo << "'." << endl;
		return 1; 
	}
	
	vector<string> lineas;
	string lineaActual;
	
	while (getline(archivo, lineaActual)) {
		lineas.push_back(lineaActual); 
	}
	
	archivo.close();
	
	for (int i = lineas.size() - 1; i >= 0; i--) {
		cout << lineas[i] << endl;
	}
	
	return 0;
}
