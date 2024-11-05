#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <cstdlib>
#include <ctime>

class TicTacToe {
private:
    std::array<std::array<char, 3>, 3> board; // Tabuleiro do jogo
    std::mutex board_mutex; // Mutex para controle de acesso ao tabuleiro
    std::condition_variable turn_cv; // Variável de condição para alternância de turnos
    char current_player; // Jogador atual ('X' ou 'O')
    bool game_over; // Estado do jogo
    char winner; // Vencedor do jogo

public:
    TicTacToe() : current_player('X'), game_over(false), winner(' ') {
        for (auto& row : board) {
            row.fill(' ');
        }
    }

void display_board() {
    std::cout << "\nTABULEIRO\n";
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            std::cout << board[row][col];
            if (col < 2) {
                std::cout << " | ";
            }
        }
        std::cout << '\n';
        if (row < 2) {
            std::cout << "---------\n";
        }
    }
    std::cout << std::endl;
}


    bool make_move(char player, int row, int col) {

        std::unique_lock<std::mutex> lock(board_mutex);

        while (!game_over && current_player != player) {
            turn_cv.wait(lock);
        }

        if (game_over) {
            return false;
        }
        
          if (board[row][col] == ' ') {
            board[row][col] = player;
            display_board();

            if (check_win(player)) {
                game_over = true;
                winner = player;
            } else if (check_draw()) {
                game_over = true;
                winner = 'D';
            }


        if (player == 'X') {
            current_player = 'O';
        } else {
            current_player = 'X';
        }

            turn_cv.notify_all();
            return true;
        }

        return false;
    }

    bool check_win(char player) {

        for (int i = 0; i < 3; i++) {
            if (board[i][0] == player && board[i][1] == player && board[i][2] == player)
                return true;
        }
        for (int j = 0; j < 3; j++) {
            if (board[0][j] == player && board[1][j] == player && board[2][j] == player) 
                return true;
        }
        
        if (board[0][0] == player && board[1][1] == player && board[2][2] == player) 
            return true;
        if (board[0][2] == player && board[1][1] == player && board[2][0] == player) 
            return true;

        return false;
    }
    
bool check_draw() {
    if(check_win(current_player)){
	return false;
    }
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == ' ') {
                return false;  
            }
        }
    }
    return true;  
}
    bool is_game_over() {
        std::unique_lock<std::mutex> lock(board_mutex);
        return game_over;
    }

    char get_winner() {
        return winner;
    }
};

// Classe Player
class Player {
private:
    TicTacToe& game; // Referência para o jogo
    char symbol; // Símbolo do jogador ('X' ou 'O')
    std::string strategy; // Estratégia do jogador

public:
    Player(TicTacToe& g, char s, const std::string& strat)
        : game(g), symbol(s), strategy(strat) {
        std::srand(std::time(0)); // Inicializar a semente do gerador de números aleatórios
    }

    void play() {
        while (!game.is_game_over()) {

            if (strategy == "sequencial") {
                play_sequential();
            } else {
                play_random();
            }

        }
    }
private:
    void play_sequential() {
         for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                if (game.make_move(symbol, row, col)) {
                    return;
                }
            }
        }
    }

    void play_random() {
        
        int col, row;
        while (!game.is_game_over()) {
            row = std::rand() % 3;
            col = std::rand() % 3;  
            if (game.make_move(symbol, row, col)) {
                return;
            }
            if (game.is_game_over()) {
                return;
            }
        }
    }
};

// Função principal
int main() {
    // Inicializar o jogo
    TicTacToe jogo;

    // Inicializar os jogadores
    Player jogador1(jogo, 'X', "sequential");
    Player jogador2(jogo, 'O', "random");

    // Criar as threads para os jogadores
    std::thread t1(&Player::play, &jogador1);
    std::thread t2(&Player::play, &jogador2);

    // Aguardar o término das threads
    t1.join();
    t2.join();

    // Exibir o resultado final do jogo
    std::cout << "Fim de Jogo" << std::endl;
    if (jogo.get_winner() == 'D') {
        std::cout << "Jogo terminou em Empate 'D' " << std::endl;
    } else {
        std::cout << "O jogador " << jogo.get_winner() << " foi o vencedor!" << std::endl;
    }

    return 0;
}