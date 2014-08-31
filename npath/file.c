#include "file.h"
#include "configure.h"

void get_path( char* path, const char* file ){
  sprintf( path, "%s%s", DIR, file );
}

int check_file( const char* file ){
  FILE* fp = fopen( file, "r" );
  if( fp == 0 ) return 0;
  fclose( fp );
  return 1;
}

int draw_file( WINDOW* win, const char* file, int y, int x ){
  char path[120];
  get_path( path, file );

  int w, h;
  getmaxyx( win, h, w );

  if( !check_file( path ) ) return 0;
  if( win == 0 ) win = stdscr;
  int ix = x, iy = y;
  char ch;
  FILE* fp = fopen( path, "r" );

  while( ( ch = fgetc( fp ) ) != EOF ){
    if( ch == '\n' ){
      iy++; ix = x;
    } else {
      if( ix >= 0 && ix < w && iy >= 0 && iy < h )
        mvwaddch( win, iy, ix, ch );
      ix++;
    }
  }
  fclose( fp );
  return 1;
}
