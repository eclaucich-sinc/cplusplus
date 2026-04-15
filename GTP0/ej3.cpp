#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
	
	string nombreArchivo = "texto.txt"; 
	
	string palabraObjetivo = "dolor"; 
	
	ifstream archivo(nombreArchivo);
	
	if (!archivo.is_open()) {
		cerr << "Error: No se pudo abrir el archivo '" << nombreArchivo << "'." << endl;
		return 1; 
	}
	
	string palabraLeida;
	int contadorOcurrencias = 0;
	
	while (archivo >> palabraLeida) {
		if (palabraLeida == palabraObjetivo) {
			contadorOcurrencias++;
		}
	}
	
	archivo.close();
	
	cout << "La palabra '" << palabraObjetivo << "' aparece " << contadorOcurrencias << " veces en el archivo." << endl;
	
	return 0;
}
