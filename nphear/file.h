#ifndef FILE_H
#define FILE_H

#include <ncurses.h>

void get_path( char path[], const char* file );
int check_file( const char* file );
int draw_file( WINDOW* win, const char* file, int y, int x );


#endif  // FILE_H
