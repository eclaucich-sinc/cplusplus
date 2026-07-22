#include <iostream>
#include <vector>

using namespace std;

class inthasher_t {
public:
    virtual void reset() = 0;
    virtual void hash(int x) = 0;
    virtual int val() = 0;
    virtual ~inthasher_t() {}
};

class sumhasher : public inthasher_t {
private:
    int sum;
public:
    sumhasher() {
        reset();
    }
    
    void reset() override {
        sum = 0;
    }
    
    void hash(int x) override {
        // En C++ el overflow de signed int es Undefined Behavior,
        // pero lo convertimos a unsigned para garantizar determinismo absoluto.
        unsigned int u_sum = static_cast<unsigned int>(sum);
        unsigned int u_x = static_cast<unsigned int>(x);
        sum = static_cast<int>(u_sum + u_x);
    }
    
    int val() override {
        return sum;
    }
};

class cubhasher : public inthasher_t {
private:
    int s;
    const unsigned int c1 = 56789234;
    const unsigned int c2 = 12345678;
public:
    cubhasher() {
        reset();
    }
    
    void reset() override {
        s = 0;
    }
    
    void hash(int x) override {
        // Usamos aritmética unsigned para evitar Undefined Behavior al desbordar
        unsigned int u_s = static_cast<unsigned int>(s);
        unsigned int u_x = static_cast<unsigned int>(x);
        unsigned int base = c1 + c2 * u_s + u_x;
        u_s = base * base * base;
        s = static_cast<int>(u_s);
    }
    
    int val() override {
        return s;
    }
};

int main() {
    sumhasher h_sum;
    cubhasher h_cub;

    vector<int> data1_base = {1, 2, 3, 4, 5, 10, 20, 100};
    vector<int> data2_same = {1, 2, 3, 4, 5, 10, 20, 100};
    vector<int> data2_perm = {1, 3, 2, 4, 5, 10, 20, 100}; // 2 y 3 intercambiados
    vector<int> data2_diff = {1, 2, 3, 4, 5, 10, 20, 99};  // 100 cambiado a 99

    auto run_hash = [&](const string& title, const vector<int>& v1, const vector<int>& v2) {
        cout << "=== " << title << " ===" << endl;
        
        cout << "Vector 1: ";
        h_sum.reset(); h_cub.reset();
        for (int x : v1) { cout << x << " "; h_sum.hash(x); h_cub.hash(x); }
        cout << "\n -> sumhasher: " << h_sum.val() << " | cubhasher: " << h_cub.val() << endl;

        cout << "Vector 2: ";
        h_sum.reset(); h_cub.reset();
        for (int x : v2) { cout << x << " "; h_sum.hash(x); h_cub.hash(x); }
        cout << "\n -> sumhasher: " << h_sum.val() << " | cubhasher: " << h_cub.val() << endl;
        cout << "\n";
    };

    run_hash("Caso 1: Vectores Iguales", data1_base, data2_same);
    run_hash("Caso 2: Vectores con posiciones intercambiadas", data1_base, data2_perm);
    run_hash("Caso 3: Vectores diferentes", data1_base, data2_diff);

    return 0;
}
