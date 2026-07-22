#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include "../GTP6/json.hpp" // json.hpp del GTP anterior

using namespace std;
using json = nlohmann::json;

// --- Clases hasher ---
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
    sumhasher() { reset(); }
    void reset() override { sum = 0; }
    void hash(int x) override {
        unsigned int u_sum = static_cast<unsigned int>(sum);
        unsigned int u_x = static_cast<unsigned int>(x);
        sum = static_cast<int>(u_sum + u_x);
    }
    int val() override { return sum; }
};

class cubhasher : public inthasher_t {
private:
    int s;
    const unsigned int c1 = 56789234;
    const unsigned int c2 = 12345678;
public:
    cubhasher() { reset(); }
    void reset() override { s = 0; }
    void hash(int x) override {
        unsigned int u_s = static_cast<unsigned int>(s);
        unsigned int u_x = static_cast<unsigned int>(x);
        unsigned int base = c1 + c2 * u_s + u_x;
        u_s = base * base * base;
        s = static_cast<int>(u_s);
    }
    int val() override { return s; }
};
// -------------------------

// Retorna la cantidad de vectores diferentes usando el hasher provisto
int count_different(vector<vector<int>> &vectors, inthasher_t &hasher) {
    set<int> unique_hashes;
    
    for (const auto& vec : vectors) {
        hasher.reset();
        for (int x : vec) {
            hasher.hash(x);
        }
        unique_hashes.insert(hasher.val());
    }
    
    return unique_hashes.size();
}

int main() {
    ifstream ifs("countdiff.json");
    if (!ifs.is_open()) {
        cerr << "Error al abrir countdiff.json" << endl;
        return 1;
    }

    json j_cases;
    ifs >> j_cases;
    ifs.close();

    cubhasher hasher;

    int passed = 0;
    int failed = 0;

    cout << "Evaluando casos de countdiff.json usando cubhasher...\n";
    cout << "--------------------------------------------------------\n";
    
    for (const auto& ucase : j_cases) {
        int case_id = ucase["ucase"];
        int expected_count = ucase["output"]["count"];
        vector<vector<int>> vectors = ucase["datain"]["vectors"].get<vector<vector<int>>>();
        
        int calculated_count = count_different(vectors, hasher);
        
        if (calculated_count == expected_count) {
            cout << "Caso " << case_id << ": OK (count=" << calculated_count << ")" << endl;
            passed++;
        } else {
            cout << "Caso " << case_id << ": FALLO (esperado=" << expected_count 
                 << ", calculado=" << calculated_count << ")" << endl;
            failed++;
        }
    }
    
    cout << "--------------------------------------------------------\n";
    cout << "Resumen: " << passed << " casos pasaron, " << failed << " fallaron." << endl;

    return 0;
}
