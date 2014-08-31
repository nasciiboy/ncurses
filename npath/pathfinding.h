#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "common.h"

int path_find( const char map[MAP_HSIZE][MAP_WSIZE], char arrow_map[MAP_HSIZE][MAP_WSIZE],
               gps init );

#endif  // PATHFINDING_H
