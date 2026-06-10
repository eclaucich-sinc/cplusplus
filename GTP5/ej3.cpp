#include <iostream>
#include <vector>
#include <string>

using namespace std;

template <typename T>
class vector_t {
    public:
        virtual ~vector_t() = default;
        virtual int size() const = 0;
        virtual T operator[](int i) const = 0;
};

// Implementación de las funciones genéricas polimórficas con templates
template <typename T>
int non_null(const vector_t<T> &v) {
    int count = 0;
    T null_val = T(); // Valor por defecto del tipo (ej: 0 para int, "" para string)
    for (int i = 0; i < v.size(); ++i) {
        if (v[i] != null_val) count++;
    }
    return count;
}

template <typename T>
T sum(const vector_t<T> &v) {
    if (v.size() == 0) return T();
    T total = v[0];
    for (int i = 1; i < v.size(); ++i) {
        total = total + v[i];
    }
    return total;
}

template <typename T>
T max(const vector_t<T> &v) {
    if (v.size() == 0) return T();
    T current_max = v[0];
    for (int i = 1; i < v.size(); ++i) {
        if (v[i] > current_max) current_max = v[i];
    }
    return current_max;
}

template <typename T>
T min(const vector_t<T> &v) {
    if (v.size() == 0) return T();
    T current_min = v[0];
    for (int i = 1; i < v.size(); ++i) {
        if (v[i] < current_min) current_min = v[i];
    }
    return current_min;
}

// 1. Vector lleno tradicional
template <typename T>
class fullvec_t : public vector_t<T> {
    private:
        vector<T> w;
    public:
        fullvec_t(int sz, T default_val = T()) : w(sz, default_val) {}
        fullvec_t(const vector<T>& vec) : w(vec) {}

        int size() const override {
            return w.size();
        }

        T operator[](int i) const override {
            return w[i];
        }
};

// 2. Vector Ralo (Sparse)
template <typename T>
class sparse_t : public vector_t<T> {
    private:
        int sz;
        vector<int> indx;
        vector<T> vals;
    public:
        sparse_t(int sz, const vector<int>& indx, const vector<T>& vals) 
            : sz(sz), indx(indx), vals(vals) {}

        int size() const override {
            return sz;
        }

        T operator[](int i) const override {
            for (size_t k = 0; k < indx.size(); ++k) {
                if (indx[k] == i) {
                    return vals[k];
                }
            }
            return T();
        }
};

// 3. Clases de asociación (Reduce)
template <typename T>
class assoc_t {
    public:
        virtual ~assoc_t() = default;
        virtual T g(T x, T y) = 0;
        virtual T null() = 0;
};

template <typename T>
class sum_t : public assoc_t<T> {
    public:
        T g(T x, T y) override { return x + y; }
        T null() override { return T(); }
};

template <typename T>
T reduce(const vector_t<T> &v, assoc_t<T> &op) {
    if (v.size() == 0) return op.null();
    
    T result = op.null();
    for (int i = 0; i < v.size(); ++i) {
        result = op.g(result, v[i]);
    }
    return result;
}

int main() {
    // Prueba con double
    fullvec_t<double> fv_d({1.0, 0.0, -3.0, 4.0, 0.0});
    cout << "--- Full Vector <double> ---" << endl;
    cout << "Non nulls: " << non_null(fv_d) << " | Sum: " << sum(fv_d) << " | Max: " << max(fv_d) << " | Min: " << min(fv_d) << endl;

    // Prueba con string (operator+ concatena, > y < son alfabéticos)
    fullvec_t<string> fv_s({"Hola", "", "Mundo", "!", ""});
    cout << "\n--- Full Vector <string> ---" << endl;
    cout << "Non nulls: " << non_null(fv_s) << " | Sum: " << sum(fv_s) << " | Max: " << max(fv_s) << " | Min: " << min(fv_s) << endl;

    // Prueba Sparse con int
    sparse_t<int> spv_i(10, {2, 5, 8}, {100, -50, 25});
    cout << "\n--- Sparse Vector <int> ---" << endl;
    cout << "Non nulls: " << non_null(spv_i) << " | Sum: " << sum(spv_i) << " | Max: " << max(spv_i) << " | Min: " << min(spv_i) << endl;

    // Prueba Reduce con string
    sum_t<string> string_sum_op;
    cout << "\n--- Prueba Reduce (Full Vector <string>) ---" << endl;
    cout << "Reduce Sum: " << reduce(fv_s, string_sum_op) << endl;

    return 0;
}
