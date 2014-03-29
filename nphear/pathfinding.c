#include "pathfinding.h"
#include <string.h>

int path_find( const char map[MAP_HSIZE][MAP_WSIZE], char arrow_map[MAP_HSIZE][MAP_WSIZE],
               gps init, gps end ){
  int change = 0;
  int y, x;
  int i, ii, dir;
  for( dir = 0; dir < 4; dir++ ){
    y = init.y + dy[dir];
    x = init.x + dx[dir];
    if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
      // out of limits
    } else {
      if( map[y][x] )
        switch( dir ){
        case RIGHT: arrow_map[y][x] = LEFT ; change = 1; break;
        case UP   : arrow_map[y][x] = DOWN ; change = 1; break;
        case LEFT : arrow_map[y][x] = RIGHT; change = 1; break;
        case DOWN : arrow_map[y][x] = UP   ; change = 1; break;
        }
      if( x == end.x && y == end.y )  return 1;
    }
  }

  while( change ){
    change = 0;
    for( i = 0; i < MAP_HSIZE; i++ )
      for( ii = 0; ii < MAP_WSIZE; ii++ ){
        if( arrow_map[i][ii] < 4 ){
          for( int dir = 0; dir < 4; dir++ ){
            y = i  + dy[dir];
            x = ii + dx[dir];
            if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
              // out of limits
            } else {
              if( map[y][x] && arrow_map[y][x] == 4 )
                switch( dir ){
                case RIGHT: arrow_map[y][x] = LEFT ; change = 1; break;
                case UP   : arrow_map[y][x] = DOWN ; change = 1; break;
                case LEFT : arrow_map[y][x] = RIGHT; change = 1; break;
                case DOWN : arrow_map[y][x] = UP   ; change = 1; break;
                }
              if( x == end.x && y == end.y )  return 1;
            }
          }
        }
      }
  }

  return 0;
}

void enigma( const char map[MAP_HSIZE][MAP_WSIZE], gps init, gps end, gps* next ){
  int y = end.y;
  int x = end.x;

  while( !( y == init.y && x == init.x ) ){
    next->y = y;
    next->x = x;

    switch( map[y][x] ){
    case RIGHT: x++; break;
    case UP   : y--; break;
    case LEFT : x--; break;
    case DOWN : y++; break;
    }
  }
}

int path( char map[MAP_HSIZE][MAP_WSIZE], gps init, gps end, gps* next){
  char arrow_map[MAP_HSIZE][MAP_WSIZE];
  memset( arrow_map, 4, sizeof(char) * MAP_HSIZE * MAP_WSIZE );

  if( path_find( map, arrow_map, init, end ) ){
    enigma( arrow_map, init, end, next );
    return 1;
  }

  return 0;
}
