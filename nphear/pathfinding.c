#include "pathfinding.h"
#include <string.h>

int path_find( const char map[MAP_HSIZE][MAP_WSIZE], char arrow_map[MAP_HSIZE][MAP_WSIZE],
               gps init ){
  memset( arrow_map, 4, sizeof(char) * MAP_HSIZE * MAP_WSIZE );
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
    }
  }

  while( change ){
    change = 0;
    // RIGHT -> DOWN ->  -- UP -- LEFT --
    for( i = 0; i < MAP_HSIZE; i++ )
      for( ii = 0; ii < MAP_WSIZE; ii++ ){
        if( arrow_map[i][ii] < 4 ){
          y = i  + dy[UP];
          x = ii + dx[UP];
          if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
            // out of limits
          } else {
            if( map[y][x] && arrow_map[y][x] == 4 ){
              arrow_map[y][x] = DOWN ; change = 1;
            }
          }     // else

          y = i  + dy[LEFT];
          x = ii + dx[LEFT];
          if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
            // out of limits
          } else {
            if( map[y][x] && arrow_map[y][x] == 4 ){
              arrow_map[y][x] = RIGHT; change = 1;
            }
          }       // else
        }         // if( arrow_map < 4 )
      }           // for( ii )

    // LEFT -> UP ->  -- DOWN -- RIGHT --
    for( i = MAP_HSIZE - 1; i >= 0; i-- )
      for( ii = MAP_WSIZE - 1; ii >= 0; ii-- ){
        if( arrow_map[i][ii] < 4 ){
          y = i  + dy[DOWN];
          x = ii + dx[DOWN];
          if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
            // out of limits
          } else {
            if( map[y][x] && arrow_map[y][x] == 4 ){
              arrow_map[y][x] = UP; change = 1;
            }
          }     // else

          y = i  + dy[RIGHT];
          x = ii + dx[RIGHT];
          if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
            // out of limits
          } else {
            if( map[y][x] && arrow_map[y][x] == 4 ){
              arrow_map[y][x] = LEFT; change = 1;
            }
          }       // else
        }         // if( arrow_map < 4 )
      }           // for( ii )

  }               // while( change )

  return 0;
}
