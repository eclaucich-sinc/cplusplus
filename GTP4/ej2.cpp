

#include <vector>
#include <limits>
#include <iostream>

using namespace std;

class vector_t {
    public:
        virtual ~vector_t() = default;
        virtual int size() = 0;
        virtual double operator[ ](int i) = 0;
};

// Implementación de las funciones genéricas polimórficas
int non_null(vector_t &v) {
    int count = 0;
    for (int i = 0; i < v.size(); ++i) {
        if (v[i] != 0.0) count++;
    }
    return count;
}

double sum(vector_t &v) {
    double total = 0.0;
    for (int i = 0; i < v.size(); ++i) {
        total += v[i];
    }
    return total;
}

double max(vector_t &v) {
    if (v.size() == 0) return numeric_limits<double>::lowest();
    double current_max = v[0];
    for (int i = 1; i < v.size(); ++i) {
        if (v[i] > current_max) current_max = v[i];
    }
    return current_max;
}

double min(vector_t &v) {
    if (v.size() == 0) return numeric_limits<double>::max();
    double current_min = v[0];
    for (int i = 1; i < v.size(); ++i) {
        if (v[i] < current_min) current_min = v[i];
    }
    return current_min;
}

// 1. Vector lleno tradicional (Dense)
class fullvec_t : public vector_t {
    private:
        vector<double> w;
    public:
        fullvec_t(int sz) : w(sz, 0.0) {}
        fullvec_t(const vector<double>& vec) : w(vec) {}

        int size() override {
            return w.size();
        }

        double operator[](int i) override {
            return w[i];
        }
};

// 2. Vector Stride (start + j * inc)
class stride_t : public vector_t {
    private:
        double start;
        double inc;
        int sz;
    public:
        stride_t(double start, double inc, int sz) : start(start), inc(inc), sz(sz) {}

        int size() override {
            return sz;
        }

        double operator[](int i) override {
            return start + i * inc;
        }
};

// 3. Vector Ralo (Sparse)
class sparse_t : public vector_t {
    private:
        int sz;
        vector<int> indx;
        vector<double> vals;
    public:
        sparse_t(int sz, const vector<int>& indx, const vector<double>& vals) 
            : sz(sz), indx(indx), vals(vals) {}

        int size() override {
            return sz;
        }

        double operator[](int i) override {
            // Buscamos si el índice existe en los valores definidos
            for (size_t k = 0; k < indx.size(); ++k) {
                if (indx[k] == i) {
                    return vals[k];
                }
            }
            // Si es un índice no registrado, implícitamente vale nulo (0.0)
            return 0.0;
        }
};

class assoc_t {
    public:
        virtual double g(double x,double y)=0;
        virtual double null()=0;
};


class sum_t : public assoc_t {
    public:
        double g(double x,double y) override { return x+y; }
        double null() override { return 0; }
};

class max_t : public assoc_t {
    public:
        double g(double x,double y) override { return (x > y) ? x : y; }
        double null() override { return numeric_limits<double>::lowest(); }
};

class min_t : public assoc_t {
    public:
        double g(double x,double y) override { return (x < y) ? x : y; }
        double null() override { return numeric_limits<double>::max(); }
};

double reduce(vector_t &v, assoc_t &op) {
    if (v.size() == 0) return op.null();
    
    double result = op.null();
    for (int i = 0; i < v.size(); ++i) {
        result = op.g(result, v[i]);
    }
    return result;
}


int main() {
    fullvec_t fv({1.0, 0.0, -3.0, 4.0, 0.0});
    stride_t sv(0, 0.5, 5); // 0, 0.5, 1.0, 1.5, 2.0
    sparse_t spv(100, {5, 50, 99}, {-3.0, 0.0, 8.0}); // Vector de 100 ptos, solo 3 elementos no nulos

    cout << "--- Full Vector ---" << endl;
    cout << "Non nulls: " << non_null(fv) << " | Sum: " << sum(fv) << " | Max: " << max(fv) << " | Min: " << min(fv) << endl;

    cout << "\n--- Stride Vector ---" << endl;
    cout << "Non nulls: " << non_null(sv) << " | Sum: " << sum(sv) << " | Max: " << max(sv) << " | Min: " << min(sv) << endl;

    cout << "\n--- Sparse Vector ---" << endl;
    cout << "Non nulls: " << non_null(spv) << " | Sum: " << sum(spv) << " | Max: " << max(spv) << " | Min: " << min(spv) << endl;

    // Pruebas de la función 'reduce' generalizada
    sum_t my_sum_op;
    max_t my_max_op;
    min_t my_min_op;

    cout << "\n--- Prueba Reduce (Sparse Vector) ---" << endl;
    cout << "Reduce Sum: " << reduce(spv, my_sum_op) << endl;
    cout << "Reduce Max: " << reduce(spv, my_max_op) << endl;
    cout << "Reduce Min: " << reduce(spv, my_min_op) << endl;

    return 0;
}