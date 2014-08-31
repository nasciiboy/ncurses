#ifndef EDITOR_H
#define EDITOR_H

#include <ncurses.h>
#include "common.h"

/* Stack data structure for maze backtracking */
struct stack {
  int x, y;           /* maze coordinates */
  struct stack *next; /* mother cell      */
};

int editor( int elevel );
int save( char map[MAP_HSIZE][MAP_WSIZE], int nlevel );
void draw_editor( char map[MAP_HSIZE][MAP_WSIZE], int delay );
void ehelp();
void draw_status(int obj, int lock );
void draw_chwin( WINDOW* win, int y, int x, int ch );


int push( struct stack **top, const int x, const int y );
void pop( struct stack **top );
void free_stack( struct stack **top );
int rand_neighbour( char maze[][ MAP_WSIZE ], int *const x, int *const y,
                    const int w, const int h );
int generate( char maze[][ MAP_WSIZE ], const int w, const int h );

#endif  // EDITOR_H
