#ifndef GAME_H
#define GAME_H

#include "common.h"

int game( int nlevel );
void help();
void draw_all( char map[MAP_HSIZE][MAP_WSIZE], int delay );
int move_player( char map[MAP_HSIZE][MAP_WSIZE], int* y, int* x, int move );
int move_obj( char map[MAP_HSIZE][MAP_WSIZE], int obj, int* y, int* x, int move );
int gravity( char map[MAP_HSIZE][MAP_WSIZE], int* new_bomb );
void kill_player( char map[MAP_HSIZE][MAP_WSIZE], int y, int x );
int set_chmap( char map[MAP_HSIZE][MAP_WSIZE], int y, int x, int nw, int prew );
void set_chmap_in_win( int y, int x, int ch );
int load_level( char map[MAP_HSIZE][MAP_WSIZE], int nlevel, int* y, int* x );
int kaboom( char map[MAP_HSIZE][MAP_WSIZE] );
int do_the_monster_dance( char map[MAP_HSIZE][MAP_WSIZE], int y, int x );

#endif  // GAME_H
