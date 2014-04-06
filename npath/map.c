#include "map.h"
#include "file.h"
#include <string.h>

int load_map( const char* file, char map[MAP_HSIZE][MAP_WSIZE] ){
  memset( map, EMPTY, sizeof(char) * MAP_HSIZE * MAP_WSIZE );

  char path[120];
  get_path( path, file );
  if( !check_file( path ) ) return 0;
  FILE* fp = fopen( path, "r" );

  int i = 0, ii = 0;
  char ch;
  while( ( ch = fgetc( fp ) ) != EOF ){
    if( ch == '\n' ){ 
      if( ++i >= MAP_HSIZE ){ fclose( fp ); return 1; }
      ii = 0;
    } else {
      if( ii < MAP_WSIZE ) map[i][ii] = ch;
      ii++;
    }
  }

  fclose( fp );
  return 1;
}

int save_map( const char* file, char map[MAP_HSIZE][MAP_WSIZE] ){
  char path[120];
  get_path( path, file );
  FILE* fp = fopen( path, "w" );
  if( fp == 0 ) return 0;

  int i, ii;
  for(i = 0; i < MAP_HSIZE; i++){
    for(ii = 0; ii < MAP_WSIZE; ii++)
      fputc( map[i][ii], fp );
    fputc( '\n', fp );
  }

  fclose( fp );
  return 1;
}

void draw_map( char map[MAP_HSIZE][MAP_WSIZE], int y, int x ){
  int i, ii;
  for(i = 0; i < MAP_HSIZE; i++)
    for(ii = 0; ii < MAP_WSIZE; ii++)
      switch( map[i][ii] ){
      case RIGHT  : mvaddch( y + i, x + ii, '>' | COLOR_PAIR( GB )); break;
      case UP     : mvaddch( y + i, x + ii, '^' | COLOR_PAIR( YB )); break;
      case LEFT   : mvaddch( y + i, x + ii, '<' | COLOR_PAIR( CB )); break;
      case DOWN   : mvaddch( y + i, x + ii, 'v' | COLOR_PAIR( MB )); break;
      case 4      : mvaddch( y + i, x + ii, ' ' | COLOR_PAIR( XX )); break;

      case EMPTY  : mvaddch( y + i, x + ii, EMPTY   | COLOR_PAIR( WB )            ); break;
      case DIRT   : mvaddch( y + i, x + ii, DIRT    | COLOR_PAIR( GG ) | A_NORMAL ); break;
      case STONE  : mvaddch( y + i, x + ii, STONE   | COLOR_PAIR( WB ) | A_NORMAL ); break;
      case DIAMOND: mvaddch( y + i, x + ii, DIAMOND | COLOR_PAIR( YB ) | A_BOLD   ); break;
      case WALL   : mvaddch( y + i, x + ii, WALL    | COLOR_PAIR( CB ) | A_NORMAL ); break;
      case MONEY  : mvaddch( y + i, x + ii, MONEY   | COLOR_PAIR( GB ) | A_NORMAL ); break;
      case PLAYER : mvaddch( y + i, x + ii, PLAYER  | COLOR_PAIR( WB ) | A_BOLD   ); break;
      case BOMB   : mvaddch( y + i, x + ii, BOMB    | COLOR_PAIR( XB ) | A_BOLD   ); break;
      case BOMBPK : mvaddch( y + i, x + ii, BOMBPK  | COLOR_PAIR( XB ) | A_BOLD   ); break;
      case MONSTER: mvaddch( y + i, x + ii, MONSTER | COLOR_PAIR( MB ) | A_BOLD   ); break;
      case BLOOD  : mvaddch( y + i, x + ii, BLOOD   | COLOR_PAIR( YB ) | A_BOLD   ); break;
      default     : mvaddch( y + i, x + ii, '?'                                   ); break;
      }

}

int change_chmap( char map[MAP_HSIZE][MAP_WSIZE], int y, int x, int nw, int prew ){
  if( x >= 0 && y >= 0 && x < MAP_WSIZE && y <  MAP_HSIZE ){
    char ch = map[y][x];
    if( prew == 0 ){
      map[y][x] = nw;
      return ch;
    } else if( map[y][x] == prew ){
      map[y][x] = nw;
      return 1;
    } else return ch;
  } else return 0;
}
