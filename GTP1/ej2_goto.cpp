#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>

using namespace std;

vector<int> readfile(string filename) {
	vector<int> values;
	ifstream file(filename);

	int number;
	while (file >> number) {
		values.push_back(number);
	}

	return values;
}


int main() {
    vector<int> values1 = readfile("f1.txt");
    vector<int> values2 = readfile("f2.txt");
    vector<int> values3 = readfile("f3.txt");
    vector<int> values4 = readfile("f4.txt");

    bool FOUND;
    int encontrado = -1;

    auto start = chrono::high_resolution_clock::now();

    for (int v1 : values1) {
        for(int v2 : values2) {
            for(int v3 : values3) {
                for(int v4 : values4) {
                    if (v1==v2 && v2==v3 && v3==v4) {
                        encontrado = v4;
                        goto FOUND;
                    }
                }
            }
        }
    }

    
    FOUND:
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Tiempo de ejecución: " << duration.count() << " ms" << endl;

    if (encontrado != -1) {
        cout << "Valor encontrado: " << encontrado << endl;
    } else {
        cout << "Valor no encontrado" << endl;
    }
    
    return 0;
}