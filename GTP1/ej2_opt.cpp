#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
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

    auto start = chrono::high_resolution_clock::now();

    sort(values1.begin(), values1.end());
    sort(values2.begin(), values2.end());
    sort(values3.begin(), values3.end());
    sort(values4.begin(), values4.end());

    int k1 = 0, k2 = 0, k3 = 0, k4 = 0;
    int encontrado = -1;


    if (values1[0]==values2[0] && values2[0]==values3[0] && values3[0]==values4[0]) {
        encontrado = values1[0];
    } else {
        while (k1<values1.size() && k2<values2.size() && k3<values3.size() && k4<values4.size()) {
            if (values1[k1]==values2[k2] && values2[k2]==values3[k3] && values3[k3]==values4[k4]) {
                encontrado = values1[k1];
                break;
            }

            int min_value = min({values1[k1], values2[k2], values3[k3], values4[k4]});

            if (values1[k1] == min_value) k1++;
            if (values2[k2] == min_value) k2++;
            if (values3[k3] == min_value) k3++;
            if (values4[k4] == min_value) k4++;
        }
    }

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