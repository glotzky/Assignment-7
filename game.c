#include "game.h"
#include <stdlib.h>
#include <stddef.h>

/*
Takes as input a pointer to a connect4 game board, a player (0/1), and a int column.
Places a new piece in `col` on the game board.
*/

int make_move(struct game* g, int player, int col){
    if (col < 0 || col >= BOARD_COLS) {
        return -1;
    }
    for (int row = BOARD_ROWS - 1; row >= 0; row--){
        int* cell = &g->board.cells[row * BOARD_COLS + col];
        if (*cell == -1) { 
            *cell = player; 
            g->current_player = 1 - player;
            return 0;
        }
    }
    return -1;
}

/*
Takes as input a pointer to a connect4 game board.
Returns 0 or 1 if player 0 or 1 has won respectively.
Returns -1 if there is no winner.
*/
int check_win(struct game* g) {
    int* b = g->board.cells;

    for (int row = 0; row < BOARD_ROWS; row++) {
        for (int col = 0; col < BOARD_COLS; col++) {
            int p = b[row * BOARD_COLS + col];
            if (p == -1) {
                continue;
            }
            if (col + 3 < BOARD_COLS &&
                b[row * BOARD_COLS + col + 1] == p &&
                b[row * BOARD_COLS + col + 2] == p &&
                b[row * BOARD_COLS + col + 3] == p) {
                return p;
            }
            if (row + 3 < BOARD_ROWS &&
                b[(row + 1) * BOARD_COLS + col] == p &&
                b[(row + 2) * BOARD_COLS + col] == p &&
                b[(row + 3) * BOARD_COLS + col] == p) {
                return p;
            }
            if (row + 3 < BOARD_ROWS && col + 3 < BOARD_COLS &&
                b[(row + 1) * BOARD_COLS + col + 1] == p &&
                b[(row + 2) * BOARD_COLS + col + 2] == p &&
                b[(row + 3) * BOARD_COLS + col + 3] == p) {
                return p;
            }
            if (row + 3 < BOARD_ROWS && col - 3 >= 0 &&
                b[(row + 1) * BOARD_COLS + col - 1] == p &&
                b[(row + 2) * BOARD_COLS + col - 2] == p &&
                b[(row + 3) * BOARD_COLS + col - 3] == p) {
                return p;
            }
        }
    }
    return -1;
}
 
/*
Initializes an existing game with an empty board.
*/
void new_game(struct game* g, int gameId) {
   g->gameId = gameId;
   g->current_player = 0;
   for (int i = 0; i < BOARD_ROWS * BOARD_COLS; i++){
    g->board.cells[i] = -1;
}
}


