#include <iostream>
#include <vector>
#include <set>
#include <map>

using namespace std;

// Función que calcula la diferencia simétrica generalizada
// usando un mapa para contar las apariciones de cada elemento
void diffsym(const vector<set<int>>& vecset, set<int>& output) {
    map<int, int> count;
    
    // Recorremos todos los conjuntos
    for (const auto& s : vecset) {
        // Recorremos los elementos de cada conjunto
        for (int elem : s) {
            count[elem]++; // Incrementamos el contador para este elemento
        }
    }
    
    // Limpiamos el conjunto de salida por si tenía elementos previos
    output.clear();
    
    // Nos quedamos solo con los elementos que aparecieron exactamente en 1 conjunto
    for (const auto& pair : count) {
        if (pair.second == 1) {
            output.insert(pair.first);
        }
    }
}

int main() {
    vector<set<int>> vecset;
    
    // Creamos algunos conjuntos de prueba
    set<int> A = {1, 2, 3, 4};
    set<int> B = {3, 4, 5, 6};
    set<int> C = {1, 6, 7};
    
    vecset.push_back(A);
    vecset.push_back(B);
    vecset.push_back(C);
    
    set<int> output;
    
    cout << "Conjunto A: { 1, 2, 3, 4 }" << endl;
    cout << "Conjunto B: { 3, 4, 5, 6 }" << endl;
    cout << "Conjunto C: { 1, 6, 7 }" << endl;
    
    // Calculamos la diferencia simétrica
    diffsym(vecset, output);
    
    cout << "\nDiferencia simetrica (elementos que estan en un solo conjunto):" << endl;
    cout << "{ ";
    for (int elem : output) {
        cout << elem << " ";
    }
    cout << "}" << endl;
    
    return 0;
}
