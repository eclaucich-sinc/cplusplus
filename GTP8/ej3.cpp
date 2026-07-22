#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include "../GTP6/json.hpp"

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

vector<int> compute_graph_hashes(const vector<vector<int>>& g) {
    int nv = g.size();
    vector<int> hashv(nv, 0);
    cubhasher hasher;

    for (int step = 0; step < 3 * nv; ++step) {
        vector<int> new_hashv(nv);
        for (int i = 0; i < nv; ++i) {
            hasher.reset();
            int valencia = g[i].size();
            hasher.hash(valencia);
            
            vector<int> v_hashes;
            for (int j : g[i]) {
                v_hashes.push_back(hashv[j]);
            }
            sort(v_hashes.begin(), v_hashes.end());
            
            for (int h : v_hashes) {
                hasher.hash(h);
            }
            
            new_hashv[i] = hasher.val();
        }
        hashv = new_hashv;
    }
    
    return hashv;
}

int main() {
    ifstream ifs("isograph.json");
    if (!ifs.is_open()) {
        cerr << "Error al abrir isograph.json" << endl;
        return 1;
    }

    json j_cases;
    ifs >> j_cases;
    ifs.close();

    int passed = 0;
    int failed = 0;

    cout << "Evaluando casos de isograph.json...\n";
    cout << "--------------------------------------------------------\n";
    
    for (const auto& ucase : j_cases) {
        int case_id = ucase["ucase"];
        vector<vector<int>> g1 = ucase["datain"]["g1"].get<vector<vector<int>>>();
        vector<vector<int>> g2 = ucase["datain"]["g2"].get<vector<vector<int>>>();
        vector<int> expected_perm = ucase["output"]["perm"].get<vector<int>>();
        
        vector<int> hashv1 = compute_graph_hashes(g1);
        vector<int> hashv2 = compute_graph_hashes(g2);
        
        int nv = g1.size();
        vector<int> perm(nv);
        bool valid = true;
        
        for (int j1 = 0; j1 < nv; ++j1) {
            int h = hashv1[j1];
            auto it = find(hashv2.begin(), hashv2.end(), h);
            if (it != hashv2.end()) {
                perm[j1] = distance(hashv2.begin(), it);
            } else {
                valid = false; // No se encontró el hash, error
                break;
            }
        }
        
        if (valid && perm == expected_perm) {
            cout << "Caso " << case_id << ": OK" << endl;
            passed++;
        } else {
            cout << "Caso " << case_id << ": FALLO" << endl;
            if (valid) {
                cout << "  Permutacion esperada: ";
                for (int p : expected_perm) cout << p << " ";
                cout << "\n  Permutacion obtenida: ";
                for (int p : perm) cout << p << " ";
                cout << endl;
            } else {
                cout << "  Error: No se encontro coincidencia para algun hash." << endl;
            }
            failed++;
        }
    }
    
    cout << "--------------------------------------------------------\n";
    cout << "Resumen: " << passed << " casos pasaron, " << failed << " fallaron." << endl;

    return 0;
}
