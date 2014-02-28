#ifndef GAME_H
#define GAME_H

#include "common_const.h"

int start_game();
void set_board( char board[BOARD_HEIGHT][BOARD_WIDTH], char mtx_brick[4][4],
                char brick, char y, char x );
int check_lines( char board[BOARD_HEIGHT][BOARD_WIDTH], int* score, int* lines );


#endif  // GAME_H
