#include <iostream>
#include <fstream>
#include <vector>

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
                // Al usar -1 y +1, podemos sumar los valores directamente
                int sum = b[lines[i][0]] + b[lines[i][1]] + b[lines[i][2]];
                
                if (sum == -3) return -1; // Ganaron las cruces (-1)
                if (sum ==  3) return  1; // Ganaron los círculos (1)
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
                
                // Insertamos un salto de línea cada 3 elementos para darle forma 3x3
                if ((i + 1) % 3 == 0) cout << endl;
            }
            cout << "------" << endl;
        }

        void load_board(vector<char> &plays){
            cout << "Cargando tablero..." << endl;
            clear();
            int current_player = -1; // Por regla general, el jugador -1 (Cruces) empieza.
            
            // Asumimos que 'plays' contiene los índices de las celdas (0-8) jugadas en orden.
            for (size_t i = 0; i < plays.size(); ++i) {
                cout << "Ronda " << i + 1 << ": ";
                play(current_player, plays[i], i);
                current_player *= -1; // Alternamos el turno: -1 se vuelve 1, y 1 se vuelve -1.
            }
        }
};


class dumb_t {
    public:
        int move(board_t &b, int me) {
            for (int k=0; k<9; ++k) {
                if (b.b[k] == 0) 
                    return k; // Devuelve la primera celda vacía
            }
            return -1; // No hay movimientos disponibles
        }

        string label() { return "dumb";}
};

class random_t {
    public:
        int move(board_t &b, int me) {
            vector<int> empty_cells;
            for (int k=0; k<9; ++k) {
                if (b.b[k] == 0) 
                    empty_cells.push_back(k);
            }
            if (empty_cells.empty()) return -1; // No hay movimientos disponibles
            int random_index = rand() % empty_cells.size();
            return empty_cells[random_index];
        }

        string label() { return "random";}
};

class agressive_t {
    public:
        int move(board_t &b, int me) {
            vector<int> Xmarks, Omarks;
            b.counts(Xmarks, Omarks);
            int opponent = -me;

            // Intentar ganar
            for (int i = 0; i < 8; ++i) {
                if ((me == -1 && Xmarks[i] == 2) || (me == 1 && Omarks[i] == 2)) {
                    for (int j = 0; j < 3; ++j) {
                        int cell = b.b[lines[i][j]];
                        if (cell == 0) return lines[i][j]; // Jugar en la celda vacía para ganar
                    }
                }
            }

            // Intentar bloquear al oponente
            for (int i = 0; i < 8; ++i) {
                if ((opponent == -1 && Xmarks[i] == 2) || (opponent == 1 && Omarks[i] == 2)) {
                    for (int j = 0; j < 3; ++j) {
                        int cell = b.b[lines[i][j]];
                        if (cell == 0) return lines[i][j]; // Jugar en la celda vacía para bloquear
                    }
                }
            }

            // Si no hay jugada ganadora o bloqueadora, jugar aleatoriamente
            vector<int> empty_cells;
            for (int k=0; k<9; ++k) {
                if (b.b[k] == 0) 
                    empty_cells.push_back(k);
            }
            if (empty_cells.empty()) return -1; // No hay movimientos disponibles
            int random_index = rand() % empty_cells.size();
            return empty_cells[random_index];
        }

        string label() { return "agressive";}
};

class minimax_t {
    private:
        int phi(board_t &b, int player, int playsnow) {
            // Vemos si ya hay un ganador
            int winner = b.wins();
            if (winner == player) return 1;
            if (winner == -player) return -1;
            
            // Revisamos si hay celdas vacías para seguir jugando
            bool has_empty = false;
            for (int i = 0; i < 9; ++i) {
                if (b.b[i] == 0) {
                    has_empty = true;
                    break;
                }
            }
            // Si no hay celdas vacías, empate
            if (!has_empty) return 0;
            
            // Inicializamos el mejor puntaje dependiendo de si es nuestro turno o el del oponente
            // Si es nuestro, buscamos maximizar,
            // Si es el turno del oponente, buscamos minimizar
            int best_score = (playsnow == player) ? -1000 : 1000;
            
            for (int k = 0; k < 9; ++k) {
                if (b.b[k] == 0) {
                    // Hacemos una jugada temporal para probarla
                    b.b[k] = playsnow; 
                    // Calculamos el puntaje con esta jugada hecha
                    int score = phi(b, player, -playsnow);
                    // Borramos la jugada
                    b.b[k] = 0;
                    
                    if (playsnow == player) {
                        // Maximizamos para nuestro jugador
                        best_score = max(best_score, score); 
                    } else {
                        // Minimizamos para el oponente
                        best_score = min(best_score, score); 
                    }
                }
            }
            
            return best_score;
        }

    public:
        int move(board_t &b, int me) {
            int best_score = -1000;
            int best_move = -1;
            
            for (int k = 0; k < 9; ++k) {
                if (b.b[k] == 0) {
                    b.b[k] = me; 
                    int score = phi(b, me, -me);
                    b.b[k] = 0;
                    
                    // Queremos maximizar el resultado en nuestra primera jugada
                    if (score > best_score) {
                        best_score = score;
                        best_move = k;
                    }
                }
            }
            return best_move;
        }

        string label() { return "minimax";}
};

class rule_based_t {
    public:
        int move(board_t &b, int me) {
            vector<int> Xmarks, Omarks;
            b.counts(Xmarks, Omarks);
            int opponent = -me;

            //Si hay una jugada ganadora, la juega.
            for (int i = 0; i < 8; ++i) {
                if ((me == -1 && Xmarks[i] == 2) || (me == 1 && Omarks[i] == 2)) {
                    for (int j = 0; j < 3; ++j) {
                        int cell = b.b[lines[i][j]];
                        // Jugar en la celda vacía para ganar
                        if (cell == 0) return lines[i][j]; 
                    }
                }
            }

            // Intentar bloquear al oponente
            for (int i = 0; i < 8; ++i) {
                if ((opponent == -1 && Xmarks[i] == 2) || (opponent == 1 && Omarks[i] == 2)) {
                    for (int j = 0; j < 3; ++j) {
                        int cell = b.b[lines[i][j]];
                        if (cell == 0) return lines[i][j]; // Jugar en la celda vacía para bloquear
                    }
                }
            }

            //Juega alguna celda libre en el siguiente orden: 
            // Si esta libre el centro, lo toma.
            if (b.b[4] == 0) return 4;

            // Posiciones de esquinas y sus opuestas
            int corners[4] = {0, 2, 6, 8};
            int opp_corners[4] = {8, 6, 2, 0};
            
            // Si el oponente tiene una esquina y la opuesta esta libre, la toma.
            for (int i = 0; i < 4; ++i) {
                if (b.b[corners[i]] == opponent && b.b[opp_corners[i]] == 0) {
                    return opp_corners[i];
                }
            }

            // Si hay una esquina disponible, la toma.
            for (int i = 0; i < 4; ++i) {
                if (b.b[corners[i]] == 0) return corners[i];
            }

            // Bordes
            int sides[4] = {1, 3, 5, 7};
            for (int i = 0; i < 4; ++i) {
                if (b.b[sides[i]] == 0) {
                    return sides[i];
                }
            }
            
            return -1;
        }

        string label() { return "rule_based";}
};


///// Partidos entre jugadores iguales
vector<int> match(dumb_t &playX, dumb_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; ++i) {
        srand(time(0) + i); // Cambiar la semilla para cada partida

        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(random_t &playX, random_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; ++i) {
        srand(time(0) + i); // Cambiar la semilla para cada partida
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(agressive_t &playX, agressive_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(minimax_t &playX, minimax_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(rule_based_t &playX, rule_based_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates
    
    for (int i=0;i <n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}


///// Partidos entre dumb y resto
vector<int> match(dumb_t &playX, random_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; ++i) {
        srand(time(0) + i); // Cambiar la semilla para cada partida
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(dumb_t &playX, agressive_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(dumb_t &playX, minimax_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(dumb_t &playX, rule_based_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates
    
    for (int i=0;i <n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}


///// Partidos entre random y resto
vector<int> match(random_t &playX, dumb_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; ++i) {
        srand(time(0) + i); // Cambiar la semilla para cada partida

        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(random_t &playX, agressive_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(random_t &playX, minimax_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(random_t &playX, rule_based_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates
    
    for (int i=0;i <n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

///// Partidos entre agressive y resto
vector<int> match(agressive_t &playX, dumb_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; ++i) {
        srand(time(0) + i); // Cambiar la semilla para cada partida

        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(agressive_t &playX, random_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; ++i) {
        srand(time(0) + i); // Cambiar la semilla para cada partida
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(agressive_t &playX, minimax_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(agressive_t &playX, rule_based_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates
    
    for (int i=0;i <n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}


///// Partidos entre minimax y resto
vector<int> match(minimax_t &playX, dumb_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; ++i) {
        srand(time(0) + i); // Cambiar la semilla para cada partida

        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(minimax_t &playX, random_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; ++i) {
        srand(time(0) + i); // Cambiar la semilla para cada partida
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(minimax_t &playX, agressive_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(minimax_t &playX, rule_based_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates
    
    for (int i=0;i <n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}


///// Partidos entre rule based y resto
vector<int> match(rule_based_t &playX, dumb_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; ++i) {
        srand(time(0) + i); // Cambiar la semilla para cada partida

        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(rule_based_t &playX, random_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; ++i) {
        srand(time(0) + i); // Cambiar la semilla para cada partida
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(rule_based_t &playX, agressive_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}

vector<int> match(rule_based_t &playX, minimax_t &playO, int n_matches=1, bool verbose=true) {
    vector<int> results = {0, 0, 0}; // victorias de X, victorias de O, empates

    for (int i=0; i<n_matches; i++) {
        board_t board;
        int current_player = -1; // Cruces empieza
        int round = 0;
        while (true) {
            int move;
            if (current_player == -1) {
                move = playX.move(board, current_player);
            } else {
                move = playO.move(board, current_player);
            }

            if (move == -1) {
                results[2]++; // Empate
                break;
            }
            board.play(current_player, move, round, verbose);
            if (verbose) {
                board.dump();
            }
            round++;

            int winner = board.wins();
            if (winner != 0) {
                if (winner == -1) results[0]++; // Victoria de X
                else results[1]++; // Victoria de O
                break;
            }
            
            current_player *= -1; // Alternar jugador
        }
    }
    return results;
}



// int main() {
//     int N_matches = 1000;

//     dumb_t playerX_dumb;
//     random_t playerX_random;
//     agressive_t playerX_agressive;
//     minimax_t playerX_minimax;
//     rule_based_t playerX_rule_based;

//     dumb_t playerO_dumb;
//     random_t playerO_random;
//     agressive_t playerO_agressive;
//     minimax_t playerO_minimax;
//     rule_based_t playerO_rule_based;

    
//     vector<int> results_dumb_dumb = match(playerX_dumb, playerO_dumb, N_matches, false);
//     vector<int> results_random_random = match(playerX_random, playerO_random, N_matches, false);
//     vector<int> results_agressive_agressive = match(playerX_agressive, playerO_agressive, N_matches, false);
//     vector<int> results_minimax_minimax = match(playerX_minimax, playerO_minimax, N_matches, false);
//     vector<int> results_rule_based_rule_based = match(playerX_rule_based, playerO_rule_based, N_matches, false);

//     vector<int> results_dumb_random = match(playerX_dumb, playerO_random, N_matches, false);
//     vector<int> results_dumb_agressive = match(playerX_dumb, playerO_agressive, N_matches, false);
//     vector<int> results_dumb_minimax = match(playerX_dumb, playerO_minimax, N_matches, false);
//     vector<int> results_dumb_rule_based = match(playerX_dumb, playerO_rule_based, N_matches, false);

//     vector<int> results_random_dumb = match(playerX_random, playerO_dumb, N_matches, false);
//     vector<int> results_random_agressive = match(playerX_random, playerO_agressive, N_matches, false);
//     vector<int> results_random_minimax = match(playerX_random, playerO_minimax, N_matches, false);
//     vector<int> results_random_rule_based = match(playerX_random, playerO_rule_based, N_matches, false);

//     vector<int> results_agressive_dumb = match(playerX_agressive, playerO_dumb, N_matches, false);
//     vector<int> results_agressive_random = match(playerX_agressive, playerO_random, N_matches, false);
//     vector<int> results_agressive_minimax = match(playerX_agressive, playerO_minimax, N_matches, false);
//     vector<int> results_agressive_rule_based = match(playerX_agressive, playerO_rule_based, N_matches, false);

//     vector<int> results_minimax_dumb = match(playerX_minimax, playerO_dumb, N_matches, false);
//     vector<int> results_minimax_random = match(playerX_minimax, playerO_random, N_matches, false);
//     vector<int> results_minimax_agressive = match(playerX_minimax, playerO_agressive, N_matches, false);
//     vector<int> results_minimax_rule_based = match(playerX_minimax, playerO_rule_based, N_matches, false);

//     vector<int> results_rule_based_dumb = match(playerX_rule_based, playerO_dumb, N_matches, false);
//     vector<int> results_rule_based_random = match(playerX_rule_based, playerO_random, N_matches, false);
//     vector<int> results_rule_based_agressive = match(playerX_rule_based, playerO_agressive, N_matches, false);
//     vector<int> results_rule_based_minimax = match(playerX_rule_based, playerO_minimax, N_matches, false);



//     //Guardar en un archivo de texto los resultados de cada jugador
//     ofstream results_file("tictactoe_results.txt");
//     results_file << "JugadorX,JugadorO,X,O,Empates\n";
//     results_file << playerX_dumb.label() << "," << playerO_dumb.label() << "," << results_dumb_dumb[0] << "," << results_dumb_dumb[1] << "," << results_dumb_dumb[2] << "\n";
//     results_file << playerX_random.label() << "," << playerO_random.label() << "," << results_random_random[0] << "," << results_random_random[1] << "," << results_random_random[2] << "\n";
//     results_file << playerX_agressive.label() << "," << playerO_agressive.label() << "," << results_agressive_agressive[0] << "," << results_agressive_agressive[1] << "," << results_agressive_agressive[2] << "\n";
//     results_file << playerX_minimax.label() << "," << playerO_minimax.label() << "," << results_minimax_minimax[0] << "," << results_minimax_minimax[1] << "," << results_minimax_minimax[2] << "\n";
//     results_file << playerX_rule_based.label() << "," << playerO_rule_based.label() << "," << results_rule_based_rule_based[0] << "," << results_rule_based_rule_based[1] << "," << results_rule_based_rule_based[2] << "\n";
    
//     results_file << playerX_dumb.label() << "," << playerO_random.label() << "," << results_dumb_random[0] << "," << results_dumb_random[1] << "," << results_dumb_random[2] << "\n";
//     results_file << playerX_dumb.label() << "," << playerO_agressive.label() << "," << results_dumb_agressive[0] << "," << results_dumb_agressive[1] << "," << results_dumb_agressive[2] << "\n";
//     results_file << playerX_dumb.label() << "," << playerO_minimax.label() << "," << results_dumb_minimax[0] << "," << results_dumb_minimax[1] << "," << results_dumb_minimax[2] << "\n";
//     results_file << playerX_dumb.label() << "," << playerO_rule_based.label() << "," << results_dumb_rule_based[0] << "," << results_dumb_rule_based[1] << "," << results_dumb_rule_based[2] << "\n";

//     results_file << playerX_random.label() << "," << playerO_dumb.label() << "," << results_random_dumb[0] << "," << results_random_dumb[1] << "," << results_random_dumb[2]<< "\n";
//     results_file<< playerX_random.label()<< ","<< playerO_agressive.label()<< ","<< results_random_agressive[0]<< ","<< results_random_agressive[1]<< ","<< results_random_agressive[2]<< "\n";
//     results_file<< playerX_random.label()<< ","<< playerO_minimax.label()<< ","<< results_random_minimax[0]<< ","<< results_random_minimax[1]<< ","<< results_random_minimax[2]<< "\n";
//     results_file<< playerX_random.label()<< ","<< playerO_rule_based.label()<< ","<<results_random_rule_based[0]<< ","<<results_random_rule_based[1]<< ","<<results_random_rule_based[2]<< "\n";

//     results_file << playerX_agressive.label() << "," << playerO_dumb.label() << "," << results_agressive_dumb[0] << "," << results_agressive_dumb[1] << "," << results_agressive_dumb[2] << "\n";
//     results_file << playerX_agressive.label() << "," << playerO_random.label() << "," << results_agressive_random[0] << "," << results_agressive_random[1] << "," << results_agressive_random[2] << "\n";
//     results_file << playerX_agressive.label() << "," << playerO_minimax.label() << "," << results_agressive_minimax[0] << "," << results_agressive_minimax[1] << "," << results_agressive_minimax[2] << "\n";
//     results_file << playerX_agressive.label() << "," << playerO_rule_based.label() << "," << results_agressive_rule_based[0] << "," << results_agressive_rule_based[1] << "," << results_agressive_rule_based[2] << "\n";

//     results_file << playerX_minimax.label() << "," << playerO_dumb.label() << "," << results_minimax_dumb[0] << "," << results_minimax_dumb[1] << "," << results_minimax_dumb[2] << "\n";
//     results_file << playerX_minimax.label() << "," << playerO_random.label() << "," << results_minimax_random[0] << "," << results_minimax_random[1] << "," << results_minimax_random[2] << "\n";
//     results_file << playerX_minimax.label() << "," << playerO_agressive.label() << "," << results_minimax_agressive[0] << "," << results_minimax_agressive[1] << "," << results_minimax_agressive[2] << "\n";
//     results_file << playerX_minimax.label() << "," << playerO_rule_based.label() << "," << results_minimax_rule_based[0] << "," << results_minimax_rule_based[1] << "," << results_minimax_rule_based[2] << "\n";

//     results_file << playerX_rule_based.label() << "," << playerO_dumb.label() << "," << results_rule_based_dumb[0] << "," << results_rule_based_dumb[1] << "," << results_rule_based_dumb[2] << "\n";
//     results_file << playerX_rule_based.label() << "," << playerO_random.label() << "," << results_rule_based_random[0] << "," << results_rule_based_random[1] << "," << results_rule_based_random[2]<< "\n";
//     results_file<< playerX_rule_based.label()<< ","<< playerO_agressive.label()<< ","<< results_rule_based_agressive[0]<< ","<< results_rule_based_agressive[1]<< ","<< results_rule_based_agressive[2]<< "\n";
//     results_file<< playerX_rule_based.label()<< ","<< playerO_minimax.label()<< ","<< results_rule_based_minimax[0]<< ","<< results_rule_based_minimax[1]<< ","<< results_rule_based_minimax[2]<< "\n";

//     results_file.close();

//     return 0;
// }

int main() {
    minimax_t playerX;
    agressive_t playerO;

    cout << playerX.label() << " vs " << playerO.label() << ":\n";
    vector<int> result = match(playerX, playerO, 1, true);

    if (result[0] == 1) {
        cout << "Gana X" << "\n";
    } else if (result[1] == 1) {
        cout << "Gana O" << "\n";
    } else {
        cout << "Empate" << "\n";
    }
    
    return 0;
}