#ifndef EDITOR_H
#define EDITOR_H

#include <ncurses.h>
#include "common.h"

int editor( int elevel );
int save( char map[MAP_HSIZE][MAP_WSIZE], int nlevel );
void draw_editor( char map[MAP_HSIZE][MAP_WSIZE], int delay );
void ehelp();
void draw_status(int obj, int lock );
void draw_chwin( WINDOW* win, int y, int x, int ch );

#endif  // EDITOR_H
