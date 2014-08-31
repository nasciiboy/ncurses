#ifndef SCREEN_H
#define SCREEN_H

#include "common_const.h"

void init_screen();
void restore_screen();
void init_colors();

WINDOW* create_win( const unsigned int h, const unsigned int w,
                    const unsigned int y, const unsigned int x );
WINDOW* create_wboard();
WINDOW* create_wpreview();
WINDOW* create_wscore();

void ck_mvaddch( int ck, int y, int x, const char ch );
void show_title();
void show_score( WINDOW* win, const int score, const int level, const int lines );
void show_preview( WINDOW* win, const char brick );
void clear_win( WINDOW* win );
void wait_start( WINDOW* win );
void draw_brick( WINDOW* win, char brick, char mtx_brick[4][4], char y, char x );
void clear_brick( WINDOW* win, char mtx_brick[4][4], char y, char x );
void show_board( WINDOW* win, char board[BOARD_HEIGHT][BOARD_WIDTH] );
void get_brick( char brick, char mtx_brick[4][4] );
void rotate_brick( char mtx_brick[4][4], char board[BOARD_HEIGHT][BOARD_WIDTH],
                   char* y, char* x, bool dir );
void adjust_brick( char mtx_brick[4][4] );
int check_brick( char mtx_brick[4][4], char board[BOARD_HEIGHT][BOARD_WIDTH],
                 char y, char x );
void move_brick( WINDOW* win, char board[BOARD_HEIGHT][BOARD_WIDTH], char mtx_brick[4][4],
                 char brick, char* y, char* x, char move );

#endif  // SCREEN_H
