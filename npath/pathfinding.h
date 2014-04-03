#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "common.h"

int path_find( const char map[MAP_HSIZE][MAP_WSIZE], char arrow_map[MAP_HSIZE][MAP_WSIZE],
               gps init, gps end );
void enigma( const char map[MAP_HSIZE][MAP_WSIZE], gps init, gps end, gps* next );
int path( char map[MAP_HSIZE][MAP_WSIZE], char path_map[MAP_HSIZE][MAP_WSIZE],
          gps init, gps end, gps* next);


#endif  // PATHFINDING_H
