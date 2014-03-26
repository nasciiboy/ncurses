#ifndef MAP_H
#define MAP_H

#include "common.h"
#include <ncurses.h>

int load_map( const char* file, char map[MAP_HSIZE][MAP_WSIZE] );
int save_map( const char* file, char map[MAP_HSIZE][MAP_WSIZE] );
void draw_map( char map[MAP_HSIZE][MAP_WSIZE], int y, int x );
int change_chmap( char map[MAP_HSIZE][MAP_WSIZE], int y, int x, int nw, int prew );

#endif  // MAP_H
