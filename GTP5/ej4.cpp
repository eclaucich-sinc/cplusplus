#include <iostream>
#include <vector>
#include <ctime>
#include <typeinfo> // Para std::bad_cast

using namespace std;

static int lines[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Filas
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Columnas
    {0, 4, 8}, {2, 4, 6}             // Diagonales
};

class board_t {
    public:
        vector<char> b;
        vector<char> rounds;

        board_t(){
            b.assign(9,0);
            rounds.assign(9,-1);
        }

        int count_rounds(){
            int count = 0;
            for (int i = 0; i < 9; ++i) {
                if (rounds[i] != -1) count++;
            }
            return count;
        }
        
        void clear(){
            b.assign(9, 0);
            rounds.assign(9, -1);
        }

        void counts(vector<int> &Xmarks, vector<int> &Omarks){
            Xmarks.assign(8, 0);
            Omarks.assign(8, 0);

            for(int i = 0; i < 8; ++i) {
                for(int j = 0; j < 3; ++j) {
                    if(b[lines[i][j]] == -1) Xmarks[i]++;
                    else if(b[lines[i][j]] == 1) Omarks[i]++;
                }
            }
        }

        int wins(){
            for (int i = 0; i < 8; ++i) {
                int sum = b[lines[i][0]] + b[lines[i][1]] + b[lines[i][2]];
                if (sum == -3) return -1; 
                if (sum ==  3) return  1; 
            }
            return 0;
        }
        
        void play(int player, int cell, int round=-1, bool verbose=true){
            if (verbose) {
                cout << "Jugador " << (player == -1 ? "X" : "O") << " juega en la celda " << cell << endl;
            }
            b[cell] = player;
            if (round != -1) rounds[cell] = round;
        }
        
        void dump(){
            for (int i = 0; i < 9; ++i) {
                if (b[i] == -1) cout << "X ";
                else if (b[i] == 1) cout << "O ";
                else cout << ". ";
                if ((i + 1) % 3 == 0) cout << endl;
            }
            cout << "------" << endl;
        }
};

class player_t {
    protected:
        string name;
    public:
        player_t(string id = "") : name(id) {}
        virtual int move(board_t &b, int me) = 0;
        virtual string label() { return name; }
        virtual ~player_t() = default;
};

class dumb_t : public player_t {
    public:
        dumb_t(string id = "") : player_t("dumb" + (id.empty() ? "" : "_" + id)) {}
        int move(board_t &b, int me) override {
            for (int k=0; k<9; ++k) {
                if (b.b[k] == 0) 
                    return k;
            }
            return -1;
        }
};

class random_t : public player_t {
    public:
        random_t(string id = "") : player_t("random" + (id.empty() ? "" : "_" + id)) {}
        int move(board_t &b, int me) override {
            vector<int> empty_cells;
            for (int k=0; k<9; ++k) {
                if (b.b[k] == 0) 
                    empty_cells.push_back(k);
            }
            if (empty_cells.empty()) return -1;
            int random_index = rand() % empty_cells.size();
            return empty_cells[random_index];
        }
};

class agressive_t : public player_t {
    public:
        agressive_t(string id = "") : player_t("agressive" + (id.empty() ? "" : "_" + id)) {}
        int move(board_t &b, int me) override {
            vector<int> Xmarks, Omarks;
            b.counts(Xmarks, Omarks);
            int opponent = -me;

            for (int i = 0; i < 8; ++i) {
                if ((me == -1 && Xmarks[i] == 2) || (me == 1 && Omarks[i] == 2)) {
                    for (int j = 0; j < 3; ++j) {
                        int cell = b.b[lines[i][j]];
                        if (cell == 0) return lines[i][j]; 
                    }
                }
            }
            for (int i = 0; i < 8; ++i) {
                if ((opponent == -1 && Xmarks[i] == 2) || (opponent == 1 && Omarks[i] == 2)) {
                    for (int j = 0; j < 3; ++j) {
                        int cell = b.b[lines[i][j]];
                        if (cell == 0) return lines[i][j]; 
                    }
                }
            }

            vector<int> empty_cells;
            for (int k=0; k<9; ++k) {
                if (b.b[k] == 0) empty_cells.push_back(k);
            }
            if (empty_cells.empty()) return -1; 
            int random_index = rand() % empty_cells.size();
            return empty_cells[random_index];
        }
};

class minimax_t : public player_t {
    private:
        int phi(board_t &b, int player, int playsnow) {
            int winner = b.wins();
            if (winner == player) return 1;
            if (winner == -player) return -1;
            
            bool has_empty = false;
            for (int i = 0; i < 9; ++i) {
                if (b.b[i] == 0) { has_empty = true; break; }
            }
            if (!has_empty) return 0;
            
            int best_score = (playsnow == player) ? -1000 : 1000;
            
            for (int k = 0; k < 9; ++k) {
                if (b.b[k] == 0) {
                    b.b[k] = playsnow; 
                    int score = phi(b, player, -playsnow);
                    b.b[k] = 0;
                    
                    if (playsnow == player) best_score = max(best_score, score); 
                    else best_score = min(best_score, score); 
                }
            }
            return best_score;
        }

    public:
        minimax_t(string id = "") : player_t("minimax" + (id.empty() ? "" : "_" + id)) {}
        int move(board_t &b, int me) override {
            int best_score = -1000;
            int best_move = -1;
            
            for (int k = 0; k < 9; ++k) {
                if (b.b[k] == 0) {
                    b.b[k] = me; 
                    int score = phi(b, me, -me);
                    b.b[k] = 0;
                    
                    if (score > best_score) {
                        best_score = score;
                        best_move = k;
                    }
                }
            }
            return best_move;
        }
};

class rule_based_t : public player_t {
    public:
        rule_based_t(string id = "") : player_t("rule_based" + (id.empty() ? "" : "_" + id)) {}
        int move(board_t &b, int me) override {
            vector<int> Xmarks, Omarks;
            b.counts(Xmarks, Omarks);
            int opponent = -me;

            for (int i = 0; i < 8; ++i) {
                if ((me == -1 && Xmarks[i] == 2) || (me == 1 && Omarks[i] == 2)) {
                    for (int j = 0; j < 3; ++j) {
                        if (b.b[lines[i][j]] == 0) return lines[i][j]; 
                    }
                }
            }

            for (int i = 0; i < 8; ++i) {
                if ((opponent == -1 && Xmarks[i] == 2) || (opponent == 1 && Omarks[i] == 2)) {
                    for (int j = 0; j < 3; ++j) {
                        if (b.b[lines[i][j]] == 0) return lines[i][j]; 
                    }
                }
            }

            if (b.b[4] == 0) return 4;

            int corners[4] = {0, 2, 6, 8};
            int opp_corners[4] = {8, 6, 2, 0};
            for (int i = 0; i < 4; ++i) {
                if (b.b[corners[i]] == opponent && b.b[opp_corners[i]] == 0) return opp_corners[i];
            }
            for (int i = 0; i < 4; ++i) {
                if (b.b[corners[i]] == 0) return corners[i];
            }

            int sides[4] = {1, 3, 5, 7};
            for (int i = 0; i < 4; ++i) {
                if (b.b[sides[i]] == 0) return sides[i];
            }
            return -1;
        }
};

int main() {
    srand(time(NULL));
    vector<player_t*> tournament;
    
    // Generamos 8 jugadores de tipos aleatorios
    for(int i = 1; i <= 8; i++) {
        int r = rand() % 5;
        string id = to_string(i);
        if (r == 0) tournament.push_back(new dumb_t(id));
        else if (r == 1) tournament.push_back(new random_t(id));
        else if (r == 2) tournament.push_back(new agressive_t(id));
        else if (r == 3) tournament.push_back(new minimax_t(id));
        else tournament.push_back(new rule_based_t(id));
    }
    
    // Imprimimos la lista completa solo para contexto
    cout << "=== Lista completa de jugadores generados ===" << endl;
    for(player_t* p : tournament) {
        cout << p->label() << endl;
    }
    cout << "=============================================\n" << endl;
    
    // 1er Método: Usando dynamic_cast<> con punteros
    cout << "--- Filtrando dumb_t usando dynamic_cast<> con PUNTEROS ---" << endl;
    for(player_t* p : tournament) {
        dumb_t* dumb_ptr = dynamic_cast<dumb_t*>(p);
        if (dumb_ptr != nullptr) {
            cout << "Encontrado: " << dumb_ptr->label() << endl;
        }
    }
    
    cout << "\n--- Filtrando dumb_t usando dynamic_cast<> con REFERENCIAS y EXCEPCIONES ---" << endl;
    for(player_t* p : tournament) {
        try {
            // Se desreferencia p para obtener una referencia
            dumb_t& dumb_ref = dynamic_cast<dumb_t&>(*p);
            cout << "Encontrado: " << dumb_ref.label() << endl;
        } catch (const bad_cast& e) {
            // El objeto subyacente no es un dumb_t, la excepción es capturada silenciosamente.
        }
    }

    // Limpieza de memoria
    for(player_t* p : tournament) {
        delete p;
    }
    
    return 0;
}
