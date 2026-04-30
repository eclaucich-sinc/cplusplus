#include <iostream>
#include <vector>
#include <stack>
#include <chrono>

using namespace std;

struct idgen_t {
  int last;
  stack<int> free;

  // Inicializa el generador
  void init() {
    // Último ínice libre (al comienzo es 0)
    last = 0;
    // Eliminamos todos los elementos de la pila de IDs libres
    while (!free.empty()) {
      free.pop();
    }
  }

  // Devuelve un nuevo ID
  int get() {
    // Si hay IDs libres en la pila
    if (!free.empty()) {
      int id = free.top();
      free.pop();
      return id;
    }
    // Si no hay libres, creamos uno nuevo
    int id = last;
    last++;
    return id;
  }

  // Libera un ID activo
  void release(int id) {
    free.push(id);
  }

  // Retorna la cantidad total de IDs activos
  int size() {
    return last - (int)free.size();
  }

  // Devuelve la lista de IDs activos actualmente
  vector<int> active() {
    vector<int> actives;
    stack<int> temp = free;
    vector<int> freed;
    while (!temp.empty()) {
      freed.push_back(temp.top());
      temp.pop();
    }
    
    for (int id = 0; id < last; id++) {
      bool is_free = false;
      for (int f : freed) {
        if (f == id) {
          is_free = true;
          break;
        }
      }
      if (!is_free) {
        actives.push_back(id);
      }
    }
    return actives;
  }

  // Libera todos los IDs activos
  void clear() {
    last = 0;
    while (!free.empty()) {
      free.pop();
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
  // idgen_t idgen;
  
  // vector<int> Ns = {10, 100, 1000, 10000, 50000, 100000};
  
  // for (int N : Ns) {
  //   idgen.init();
  //   auto start = chrono::high_resolution_clock::now();

  //   for (int i = 0; i < N; i++) {
  //     idgen.get();
  //   }

  //   for (int i = 0; i < N; i++) {
  //     idgen.release(i);
  //   }

  //   for (int i = 0; i < N; i++) {
  //     idgen.get();
  //   }
  
  //   auto end = chrono::high_resolution_clock::now();
  //   auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
  //   cout << "Tiempo de ejecución para " << N << ": " << duration.count() << " ms" << endl;
  // }

  // return 0;
  idgen_t idgen;
  idgen.init();

  int N = 20;

  cout << "Generando " << N << " IDs..." << endl;
  for (int i = 0; i < N; i++) {
    cout << idgen.get() << " ";
  }
  cout << endl;
  cout << "size " << idgen.size() << endl;
  dump(idgen);
  
  cout << "Liberando IDs pares..." << endl;
  for (int i = 0; i < N; i += 2) {
    idgen.release(i);
  }
  cout << "size " << idgen.size() << endl;
  dump(idgen);

  cout << "Generando 10 IDs más..." << endl;
  for (int i = 0; i < 10; i++) {
    cout << idgen.get() << " ";
  }
  cout << endl;
  cout << "size " << idgen.size() << endl;
  dump(idgen);
}
