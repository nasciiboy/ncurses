#ifndef UTILS_H
#define UTILS_H

#include <ncurses.h>

void start_curses();
void end_curses();
void clrscr();
void start_ck();
void nasciiboy();
void min_size( int y, int x );
int getopt( int argc, char * const argv[], const char *optchar );
int msgbox( const char* msg );
void print_in_middle( WINDOW* win, int starty, const char* msg );

#endif  // UTILS_H
