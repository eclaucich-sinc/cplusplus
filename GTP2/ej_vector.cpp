#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

const int IDMAX = 500;

struct idgen_t {
  vector<bool> busy;

  // Inicializa el generador
  void init() {
    // Siempre nos ajustamos a un tamaño fijo
    busy.resize(IDMAX);
    // Establecemos todos como libres al comienzo
    for (int i = 0; i < IDMAX; i++) {
      busy[i] = false;
    }
  }

  // Devuelve un nuevo ID
  int get() {
    //Recorremos el vector hasta encontrar el primero libre
    for (int id = 0; id < IDMAX; id++) {
      // Si encontramos un ID libre
      if (!busy[id]) {
        // Lo marcamos como ocupado y lo devolvemos
        busy[id] = true;
        return id;
      }
    }
    return -1;
  }

  // Libera un ID activo
  void release(int id) {
    // Verificamos que el ID sea válido antes de liberarlo
    if (0 <= id && id < IDMAX) {
      busy[id] = false;
    }
  }

  // Retorna la cantidad total de IDs activos
  int size() {
    // Simplemente contamos cuántos IDs están marcados como ocupados
    int count = 0;
    for (bool occupied : busy) {
      if (occupied) {
        count++;
      }
    }
    return count;
  }

  // Devuelve la lista de IDs activos actualmente
  vector<int> active() {
    // Guardamos todos los IDs que están marcados como ocupados en un vector y lo devolvemos
    vector<int> actives;
    for (int id = 0; id < IDMAX; id++) {
      if (busy[id]) {
        actives.push_back(id);
      }
    }
    return actives;
  }

  // Libera todos los IDs activos
  void clear() {
    for (int id = 0; id < IDMAX; id++) {
      busy[id] = false;
    }
  }
};


void dump(idgen_t &idgen) {
  vector<int> actives = idgen.active();
  cout << "IDs activos: ";
  for (int id : actives) {
    cout << id << " ";
  }
  cout << endl;
}


int main() {
  idgen_t idgen;
  
  vector<int> Ns = {10, 100, 1000, 10000, 50000, 100000};
  
  for (int N : Ns) {
    idgen.init();
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
      idgen.get();
    }

    for (int i = 0; i < N; i++) {
      idgen.release(i);
    }

    for (int i = 0; i < N; i++) {
      idgen.get();
    }
  
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Tiempo de ejecución para " << N << ": " << duration.count() << " ms" << endl;
  }

  return 0;
}
