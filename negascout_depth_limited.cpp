//
// Created by @namhainguyen2803 on 28/12/2022.
//

//g++ -std=c++17 main.cpp -o gomokuAI
// ./gomokuAI
#include <iostream>
#include <vector>
#include <limits>
#include <random>
#include <map>
#include <chrono>
#include <array>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;
using namespace std::chrono;
// 1 as X
// -1 as O
// 0 as none of them

// Initial Board game
vector<vector<int>> GameBoard(15, vector<int>(15, 0));
struct Move {
    int row;
    int col;
    int score;

    Move() {
        this->row = 0;
        this->col = 0;
        this->score = 0;
    }

    Move(int row, int col, int score) {
        this->row = row;
        this->col = col;
        this->score = score;
    }
};

struct Node {
    int flag;
    int depth;
    int score;

    Node() {
        this->flag = 0;
        this->depth = 0;
        this->score = 0;
    }

    Node(int flag, int depth, int score) {
        this->flag = flag;
        this->depth = depth;
        this->score = score;
    }
};

map<int, Node> cache_node;

map<int, int> state_cache;

int num_rows = GameBoard.size();

int num_columns = GameBoard[0].size();

int inf = 1e9;

int win_detected = numeric_limits<int>::min() + 1;;

int live_one = 10;

int dead_one = 1;

int live_two = 100;

int dead_two = 10;

int live_three = 1e5;

int dead_three = 100;

int live_four = 1e6;

int dead_four = 1000;

int five = 1e8;

int MaximumDepth = 2;

int winner;

Move Best_Move;


// Assign score to each array
// List all the possible configurations
int evaluate_block(int blocks, int pieces) {
    // case that the array has not be blocked
    if (blocks == 0) {
        if (pieces == 1) {
            return live_one;
        } else if (pieces == 2) {
            return live_two;
        } else if (pieces == 3) {
            return live_three;
        } else if (pieces == 4) {
            return live_four;
        } else {
            return five;
        }
    }
        // case that the array has been blocked one side
    else if (blocks == 1) {
        if (pieces == 1) {
            return dead_one;
        } else if (pieces == 2) {
            return dead_two;
        } else if (pieces == 3) {
            return dead_three;
        } else if (pieces == 4) {
            return dead_four;
        } else {
            return five;
        }
    }
        // case that array has been blocked by two sides
    else {
        if (pieces >= 5) {
            return five;
        } else {
            return 0;
        }
    }
}

// Use evaluate_block to calculate score
// Evaluate function that take input as a configuration of the board and evaluate the score of that configuration
// Use this evaluation funciton when reaching the MaximumDepth(almost precise)
int evaluate_board(vector<vector<int>> Board, int player, vector<int> restrictions) {
    int score = 0;
    int lowest_row = restrictions[0];
    int lowest_column = restrictions[1];
    int highest_row = restrictions[2];
    int highest_column = restrictions[3];

    // Evaluate Rows
    for (int row = lowest_row; row < highest_row + 1; row++) {
        for (int col = lowest_column; col < highest_column + 1; col++) {
            if (Board[row][col] == player) {
                // count for number of piece_type in an array and the number of block between the tail and head of array
                int block = 0;
                int piece = 1;
                if (col == 0 || Board[row][col - 1] != 0) {
                    block++;
                }
                for (col++; col < num_columns && Board[row][col] == player; col++) {
                    piece++;
                }
                if (col == num_columns || Board[row][col] != 0) {
                    block++;
                }
                score += evaluate_block(block, piece);
            }
        }
    }

    // Evaluate columns
    for (int col = lowest_column; col < highest_column + 1; col++) {
        for (int row = lowest_row; row < highest_row + 1; row++) {
            if (Board[row][col] == player) {
                int block = 0;
                int piece = 1;
                if (row == 0 || Board[row - 1][col] != 0) {
                    block++;
                }
                for (row++; row < num_rows && Board[row][col] == player; row++) {
                    piece++;
                }
                if (row == num_rows || Board[row][col] != 0) {
                    block++;
                }
                score += evaluate_block(block, piece);
            }
        }
    }

    // Evaluate left diagonal
    for (int n = lowest_row; n < (highest_column - lowest_column + highest_row); n += 1) {
        int row = n;
        int col = lowest_column;
        while (row >= lowest_row && col <= highest_column) {
            if (row <= highest_row) {
                if (Board[row][col] == player) {
                    int block = 0;
                    int piece = 1;
                    if (col == 0 || row == num_rows - 1 || Board[row + 1][col - 1] != 0) {
                        block++;
                    }
                    row--;
                    col++;
                    for (; row >= 0 && Board[row][col] == player; row--) {
                        piece++;
                        col++;
                    }
                    if (row < 0 || col == num_columns || Board[row][col] != 0) {
                        block++;
                    }
                    score += evaluate_block(block, piece);
                }
            }
            row -= 1;
            col += 1;
        }
    }

    // Evaluate right diagonal
    for (int n = lowest_row - (highest_column - lowest_column); n <= highest_row; n++) {
        int row = n;
        int col = lowest_column;
        while (row <= highest_row && col <= highest_column) {
            if (row >= lowest_row && row <= highest_row) {
                if (Board[row][col] == player) {
                    int block = 0;
                    int piece = 1;
                    if (col == 0 || row == 0 || Board[row - 1][col - 1] != 0) {
                        block++;
                    }
                    row++;
                    col++;
                    for (; row < num_rows && Board[row][col] == player; row++) {
                        piece++;
                        col++;
                    }
                    if (row == num_rows || col == num_columns || Board[row][col] != 0) {
                        block++;
                    }
                    score += evaluate_block(block, piece);
                }
            }
            row += 1;
            col += 1;
        }
    }
    return score;
}

// We restrict the region to evaluate the configuration
vector<int> get_restrictions(vector<vector<int>> Board) {
    vector<int> restrict;
    int lowest_row = numeric_limits<int>::max()-1;
    int lowest_col = numeric_limits<int>::max()-1;
    int highest_row = numeric_limits<int>::min()+1;
    int highest_col = numeric_limits<int>::min()+1;
    for (int row = 0; row < num_rows; row++) {
        for (int col = 0; col < num_columns; col++) {
            if (Board[row][col] == 1 || Board[row][col] == -1) {
                lowest_row = min(lowest_row, row);
                lowest_col = min(lowest_col, col);
                highest_row = max(highest_row, row);
                highest_col = max(highest_col, col);
            }
        }
    }

    // handle unnecessary cell, limiting the region to be maximum 13x13
    lowest_row = max(lowest_row, 2);
    lowest_col = max(lowest_col, 2);
    highest_row = min(highest_row, 12);
    highest_col = min(highest_col, 12);

    restrict.push_back(lowest_row);
    restrict.push_back(lowest_col);
    restrict.push_back(highest_row);
    restrict.push_back(highest_col);
    return restrict;
}

// Check for 5 in a row
bool check_directions(vector<int> arr) {
    if (arr.size() > 4) {
        for (int i = 0; i < arr.size() - 4; i++) {
            if (arr[i] == 1 || arr[i] == -1 ) {
                if (arr[i] == arr[i + 1] && arr[i] == arr[i + 2] && arr[i] == arr[i + 3] && arr[i] == arr[i + 4]) {
                    return true;
                }
            }
        }
    }
    return false;
}

// get 4 directions(vertical, horizontal, left diagonal, right diagonal) of a cell
vector<vector<int>> get_directions(vector<vector<int>> Board, int cor_row, int cor_col) {
    vector<vector<int>> directions;
    vector<int> vertical_direction;
    vector<int> horizontal_direction;
    vector<int> leftdiag_direction;
    vector<int> rightdiag_direction;

    for(int i = -4; i < 5; i++){
        if(cor_row + i >= 0 && cor_row + i <= num_rows - 1){
            vertical_direction.push_back(Board[cor_row+i][cor_col]);
            if(cor_col + i >= 0 && cor_col + i <= num_columns - 1){
                leftdiag_direction.push_back(Board[cor_row+i][cor_col+i]);
            }
        }
        if(cor_col+i >= 0 && cor_col+i <=num_columns -1){
            horizontal_direction.push_back(Board[cor_row][cor_col+i]);
            if(cor_row-i>=0 && cor_row-i <= num_rows-1){
                rightdiag_direction.push_back(Board[cor_row-i][cor_col+i]);
            }
        }
    }
    directions.push_back(vertical_direction);
    directions.push_back(horizontal_direction);
    directions.push_back(leftdiag_direction);
    directions.push_back(rightdiag_direction);
    return directions;
}

// Check for winning configuration or not
bool check_win(vector<vector<int>> Board, int cor_row, int cor_col) {
    vector<vector<int>> directions = get_directions(Board, cor_row, cor_col);
    for (auto &direction: directions) {
        if (check_directions(direction) == true) {
            return true;
        }
    }
    return false;
}

// Change the region of interest
vector<int> change_restrictions(vector<int> restrictions, int new_row, int new_col) {
    vector<int> restrict;
    int lowest_row = restrictions[0];
    int lowest_col = restrictions[1];
    int highest_row = restrictions[2];
    int highest_col = restrictions[3];

    // update region
    lowest_row = min(lowest_row, new_row);
    highest_row = max(highest_row, new_row);
    lowest_col = min(lowest_col, new_col);
    highest_col = max(highest_col, new_col);

    lowest_row = max(lowest_row, 2);
    lowest_col = max(lowest_col, 2);
    highest_row = min(highest_row, 12);
    highest_col = min(highest_col, 12);

    restrict.push_back(lowest_row);
    restrict.push_back(lowest_col);
    restrict.push_back(highest_row);
    restrict.push_back(highest_col);
    return restrict;
}

// Check for existence of neighbours of the cell
bool neighbour_cell(vector<vector<int>> Board, int row, int col) {
    for (int i = row - 2; i <= row + 2; i++) {
        if (i < 0 || i >= num_rows) {
            continue;
        }
        for (int j = col - 2; j <= col + 2; j++) {
            if (j < 0 || j >= num_columns) {
                continue;
            }
            if (Board[i][j] != 0) {
                return false;
            }
        }
    }
    return true;
}

int get_seq(int y, int e) {
    // player 1 and player 2
    if (y + e == 0) {
        return 0;
    }
        // player 1 and not player 2
    else if (y != 0 and e == 0) {
        return y;
    }
        // not player 1 and player 2
    else if (y == 0 and e != 0) {
        return -e;
    }
        // not player 1 and not player 2
    else {
        return 28;
    }
}

int evalff(int seq) {
    switch (seq) {
        case 0:
            return 7;
        case 1:
            return 35;
        case 2:
            return 800;
        case 3:
            return 15000;
        case 4:
            return 800000;
        case -1:
            return 15;
        case -2:
            return 400;
        case -3:
            return 1800;
        case -4:
            return 100000;
        default:
            return 0;
    }
}

// Evaluate four directions at get_directions
int evaluate_direction(vector<int> direction_arr, int player) {
    int score = 0;
    if(direction_arr.size() > 4){
        for (int i = 0; i < direction_arr.size() - 4; i++) {
            int player_1 = 0; // player
            int player_2 = 0; // enemy
            for (int j = 0; j < 5; j++) {
                if (direction_arr[i + j] == player) {
                    player_1++;
                } else if (direction_arr[i + j] == -1 * player) {
                    player_2++;
                }
            }
            score += evalff(get_seq(player_1, player_2));
            if ((score >= 800000)) {
                return win_detected;
            }
        }
    }
    return score;
}

// Call the evaluate_direction at each four directions of a cell, shallow evaluation(not precise)
// Use this evaluation function at each depth to sort every possible move
int evaluate_move(vector<vector<int>> Board, int cor_row, int cor_col, int player) {
    int score = 0;
    vector<vector<int>> directions = get_directions(Board, cor_row, cor_col);
    int temp_score = 0;
    for (int i = 0; i < 4; i++) {
        temp_score = evaluate_direction(directions[i], player);
        if (temp_score >= 800000) {
            return win_detected;
        }
        else{
            score += temp_score;
        }
    }
    return score;
}

bool compare_byscore(Move a, Move b) {
    return a.score > b.score;
}

//Generate possible future movement and sort by evaluate_move
vector<Move> BoardGenerator(vector<int> restrictions, vector<vector<int>> Board, int player) {
    vector<Move> candidate_moves;
    int lowest_row = restrictions[0];
    int lowest_col = restrictions[1];
    int highest_row = restrictions[2];
    int highest_col = restrictions[3];
    for (int i = lowest_row - 2; i <= highest_row + 2; i++) {
        for (int j = lowest_col - 2; j <= highest_col + 2; j++) {
            if ((Board[i][j] == 0) && (neighbour_cell(Board, i, j) == false)) {
                Move move;
                move.row = i;
                move.col = j;
                move.score = evaluate_move(Board, i, j, player);
                if (move.score == win_detected) {
                    vector<Move> winning_move;
                    winning_move.push_back(move);
                    return winning_move;
                }
                candidate_moves.push_back(move);
            }
        }
    }
    // Sort by evaluete_move - shallow sort
    sort(candidate_moves.begin(), candidate_moves.end(), compare_byscore);
    return candidate_moves;
}

// Zobrist hashing table(transposition table)
int Table[15][15][2];
mt19937 mt_rand((unsigned) time(nullptr));

void Table_init() {
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            Table[i][j][0] = mt_rand();
            Table[i][j][1] = mt_rand();
        }
    }
}

int Hash(vector<vector<int>> Board) {
    int h = 0;
    int p = 0;
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_columns; j++) {
            int player = Board[i][j];
            if (player != 0) {
                if (player == -1) {
                    p = 0;
                } else {
                    p = 1;
                }
                h = h ^ Table[i][j][p];
            }
        }
    }
    return h;
}

int update_hash(int hash, int player, int row, int col) {
    if (player == -1)
        hash ^= Table[row][col][0];
    else
        hash ^= Table[row][col][1];
    return hash;
}

// Use evaluate_board to calculate
int evaluate_state(vector<vector<int>> Board, int player, int hash, vector<int> restrictions) {
    int x_score = evaluate_board(Board, 1, restrictions);
    int o_score = evaluate_board(Board, -1, restrictions);
    int score = 0;
    if (player == 1) {
        score = x_score - o_score;
    } else {
        score = o_score - x_score;
    }
    state_cache[hash] = score;
    return score;
}

int negamax(vector<vector<int>> Board, int player, int depth, int alpha, int beta, int hash, vector<int> restrictions,
            int last_i, int last_j) {
    const int alpha_original = alpha;
    if ((cache_node.count(hash) != 0) && (cache_node[hash].depth >= depth)) {
        int score = cache_node[hash].score;
        if (cache_node[hash].flag ==  0) {
            return score;
        }
        if (cache_node[hash].flag ==  -1) {
            alpha = max(alpha, score);
        }
        else if (cache_node[hash].flag ==  1) {
            beta = min(beta, score);
        }
        if (alpha >= beta) {
            return score;
        }
    }
    if (check_win(Board, last_i, last_j) && last_j >= 0 && last_i >= 0) {
        return -2000000 + (MaximumDepth - depth);
    }

    // Base case
    if (depth == 0) {
        if (state_cache.count(hash) != 0) {
            return state_cache[hash];
        } else {
            return evaluate_state(Board, player, hash, restrictions);
        }
    }
    vector<Move> candidate_moves = BoardGenerator(restrictions, Board, player);
    if (candidate_moves.size() == 0) {
        return 0;
    }
    int best_value = numeric_limits<int>::min()+1;
    for (int y = 0; y < candidate_moves.size(); y++) {
        int cor_row = candidate_moves[y].row;
        int cor_col = candidate_moves[y].col;
        Board[cor_row][cor_col] = player;
        int new_hash = update_hash(hash, player, cor_row, cor_col);
        vector<int> restrictions_temp = change_restrictions(restrictions, cor_row, cor_col);
        int value = -negamax(Board, -player, depth - 1, -beta, -alpha, new_hash, restrictions_temp, cor_row, cor_col);
        Board[cor_row][cor_col] = 0;
        if (value > best_value) {
            best_value = value;
            if (depth == MaximumDepth) {
                Best_Move.row = cor_row;
                Best_Move.col = cor_col;
                Best_Move.score = value;
            }
        }
        alpha = max(alpha, value);
        if (alpha >= beta) {
            break;
        }
    }
    Node config(0, depth, best_value);
    if (best_value <= alpha_original) {
        config.flag = 1;
    } else if (best_value >= beta) {
        config.flag = -1;
    } else {
        config.flag = 0;
    }
    cache_node[hash] = config;
    return best_value;
}


int negascout(vector<vector<int>> Board, int player, int depth, int alpha, int beta, int hash, vector<int> restrictions,
              int last_i, int last_j) {

    int original_alpha = alpha;
    if ((cache_node.count(hash) != 0) && (cache_node[hash].depth >= depth)) {
        int score = cache_node[hash].score;
        switch (cache_node[hash].flag) {
            case 1:
                beta = min(beta, score);
                break;
            case -1:
                alpha = max(alpha, score);
                break;
            default:
                return score;
        }
        if (alpha >= beta) {
            return score;
        }
    }
    if (check_win(Board, last_i, last_j) && last_j >= 0 && last_i >= 0) {
        return -2000000 + (MaximumDepth - depth);
    }

    // Base case
    if (depth == 0) {
        if (state_cache.count(hash) != 0) {
            return state_cache[hash];
        } else {
            return evaluate_state(Board, player, hash, restrictions);
        }
    }
    vector<Move> candidate_moves = BoardGenerator(restrictions, Board, player);
    if (candidate_moves.size() == 0) {
        return 0;
    }
    int b = beta;
    int best_value = -inf;
    for (int y = 0; y < candidate_moves.size(); y++) {
        int cor_row = candidate_moves[y].row;
        int cor_col = candidate_moves[y].col;
        Board[cor_row][cor_col] = player;
        int new_hash = update_hash(hash, player, cor_row, cor_col);
        vector<int> restrictions_temp = change_restrictions(restrictions, cor_row, cor_col);
        int value = -negascout(Board, -player, depth - 1, -b, -alpha, new_hash, restrictions_temp, cor_row, cor_col);
        if (alpha < value and value < beta and y > 0) {
            value = -negascout(Board, -player, depth - 1, -beta, -value, new_hash, restrictions_temp, cor_row, cor_col);
        }
        if (value > best_value) {
            best_value = value;
            if (depth == MaximumDepth) {
                Best_Move.row = cor_row;
                Best_Move.col = cor_col;
                Best_Move.score = value;
            }
        }
        Board[cor_row][cor_col] = 0;
        alpha = max(alpha, value);
        if (alpha >= beta) {
            break;
        }
        b = alpha + 1;
    }
    Node config(0, depth, best_value);
    if (best_value <= original_alpha) {
        config.flag = 1;
    } else if (best_value >= b) {
        config.flag = -1;
    } else {
        config.flag = 0;
    }
    cache_node[hash] = config;
    return best_value;
}

Move iterative_negamax(int player, vector<vector<int>> Board) {
    Table_init();
    Move best_move(-1, -1, 0);
    int depth = 4;
    MaximumDepth = depth;
    int temp_bestscore;
    temp_bestscore = negamax(Board, player, MaximumDepth, numeric_limits<int>::min()+1, numeric_limits<int>::max()-1, Hash(Board)-1, get_restrictions(Board), 0, 0);
    best_move = Best_Move;
//    cout <<"Best Move negamax: " << best_move.row << " " << best_move.col << endl;
    cache_node.clear();
    state_cache.clear();
    return best_move;
}

Move iterative_negascout(int player, vector<vector<int>> Board) {
    Table_init();
    Move best_move(-1, -1, 0);
    int depth = 4;
    MaximumDepth = depth;
    int temp_bestscore;
    temp_bestscore = negascout(Board, player, MaximumDepth, -inf, inf, Hash(Board), get_restrictions(Board), -1,
                               -1);
    best_move = Best_Move;
    cache_node.clear();
    state_cache.clear();
//    cout <<"Best Move negascout: " << best_move.row << " " << best_move.col << endl;
    return best_move;
}


bool terminal(vector<vector<int>> Board) {
    int cnt = 0;
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_columns; j++) {
            if (check_win(Board, i, j) == true) {
                cout << i  << " " << j  << endl;
                winner = Board[i][j];
                return true;
            }
            if (Board[i][j] != 0) {
                cnt += 1;
            }
        }
    }
    if (cnt == num_rows * num_columns) {
        winner = 0;
        return true;
    }
    return false;
}

void PrintBoard(vector<vector<int>> &Board){
    system("clear");
    cout << "                       1 1 1 1 1" << endl;
    cout << "   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4" << endl;
    for(int i = 0; i < num_rows; i++) {
        if(i < 10)
            cout << " " << i << " ";
        else
            cout << i << " ";
        for(int j = 0; j < num_columns; j++) {
            if(Board[i][j] == 0)
                cout << "_ ";
            else if (Board[i][j] == 1)
                cout << "X ";
            else
                cout << "O ";
        }
        cout << endl;
    }
    return;
}

int main() {
    int human;
    cout<<"Play as [x/o]: ";
    char crt;
    cin >> crt;
    cout<< "Select the opponent [negamax/negascout]: ";
    string opponent;
    cin >> opponent;
    if (crt == 'x') {
        human = 1;
        PrintBoard(GameBoard);
        while (terminal(GameBoard) == false) {
            int r;
            int c;
            cout << "Input the coordinate: ";
            cin >> r;
            cin >> c;
            while(GameBoard[r][c] != 0){
                cout << "cell " << "(" << r << "," << c << ")" << " has been selected, please select another move"<<endl;
                cout << "Input the coordinate: ";
                cin >> r;
                cin >> c;
            }
            GameBoard[r][c] = 1;
            PrintBoard(GameBoard);
            cout <<"You as X played "<< r << " " << c << endl;
            Move best_move;
            if(opponent == "negascout"){
                best_move = iterative_negascout(-1, GameBoard);
            }
            else{
                best_move = iterative_negamax(-1, GameBoard);
            }
            int o_r = best_move.row;
            int o_c = best_move.col;
            GameBoard[o_r][o_c] = -1;
            PrintBoard(GameBoard);
            cout <<"Computer as O played "<< o_r << " " << o_c << endl;
            GameBoard[o_r][o_c] = -1;

        }
    } else if (crt == 'o') {
        human = -1;
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        std::uniform_int_distribution<> distr(4, 10); // define the range
        int res1 = distr(gen);
        int res2 = distr(gen);
        GameBoard[res1][res2] = 1;
        PrintBoard(GameBoard);
        cout <<"Computer as X played "<< res1 << " " << res2 << endl;
        int ro;
        int co;
        cout << "Input the coordinate: ";
        cin >> ro;
        cin >> co;
        while(GameBoard[ro][co] != 0){
            cout << "cell " << "(" << ro << "," << co << ")" << " has been selected, please select another move"<<endl;
            cout << "Input the coordinate: ";
            cin >> ro;
            cin >> co;
        }
        GameBoard[ro][co] = -1;
        PrintBoard(GameBoard);
        cout <<"You as O played "<< ro << " " << co << endl;
        while (terminal(GameBoard) == false) {
            Move best_move;
            if(opponent == "negascout"){
                best_move = iterative_negascout(-1, GameBoard);
            }
            else{
                best_move = iterative_negamax(-1, GameBoard);
            }
            int o_ro = best_move.row;
            int o_co = best_move.col;
            GameBoard[o_ro][o_co] = 1;
            PrintBoard(GameBoard);
            cout <<"Computer as X played "<< o_ro << " " << o_co << endl;
            int ro;
            int co;
            cout << "Input the coordinate: ";
            cin >> ro;
            cin >> co;
            while(GameBoard[ro][co] != 0){
                cout << "cell " << "(" << ro << "," << co << ")" << " has been selected, please select another move"<<endl;
                cout << "Input the coordinate: ";
                cin >> ro;
                cin >> co;
            }
            GameBoard[ro][co] = -1;
            PrintBoard(GameBoard);
            cout <<"You as O played "<< ro << " " << co << endl;
        }
    }
    PrintBoard(GameBoard);
    if(winner == human){
        cout << "Human won.";
    }
    else if(winner == (-1) * human){
        cout<< "Computer won.";
    }
    else{
        cout<<"Draws";
    }

    return 0;
}
