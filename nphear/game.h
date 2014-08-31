#ifndef GAME_H
#define GAME_H

#include "common.h"

int game( int nlevel );
void help();
void draw_all( char map[MAP_HSIZE][MAP_WSIZE], int delay, LSDB lsdb );
int move_player( char map[MAP_HSIZE][MAP_WSIZE], gps* player, int move, LSDB* lsdb );
int move_obj( char map[MAP_HSIZE][MAP_WSIZE], int obj, int* y, int* x, int move );
int gravity( char map[MAP_HSIZE][MAP_WSIZE], int* new_bomb );
void kill_player( char map[MAP_HSIZE][MAP_WSIZE], gps player );
int load_level( char map[MAP_HSIZE][MAP_WSIZE], int nlevel, gps* player, int* diamonds );
int kaboom( char map[MAP_HSIZE][MAP_WSIZE] );
int do_the_monster_dance( char map[MAP_HSIZE][MAP_WSIZE], gps player );

#endif  // GAME_H
