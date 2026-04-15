#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Paso por puntero
void dup(vector<int> *a) {
	if (a == nullptr || a->empty()) return; 
	
	auto it = max_element(a->begin(), a->end());
	
	*it = (*it) * 2; 
}

// Paso por referencia
void dup(vector<int> &a) {
	if (a.empty()) return;
	
	auto it = max_element(a.begin(), a.end());
	*it = (*it) * 2; 
}

// Retorno de valor
vector<int> dup_return(vector<int> a) {
	if (a.empty()) return a;
	
	auto it = max_element(a.begin(), a.end());
	*it = (*it) * 2; 
	
	return a; 
}

int main() {
	vector<int> v1 = {1, 5, 3};
	vector<int> v2 = {1, 5, 3};
	vector<int> v3 = {1, 5, 3};
	
	dup(&v1);
	cout << "Puntero: ";
	for(int x : v1) cout << x << " ";
	cout << endl;
	
	dup(v2);
	cout << "Referencia: ";
	for(int x : v2) cout << x << " ";
	cout << endl;
	
	v3 = dup_return(v3);
	cout << "Retorno: ";
	for(int x : v3) cout << x << " ";
	cout << endl;
	
	return 0;
}
