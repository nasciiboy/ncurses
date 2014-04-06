#include "file.h"
#include "map.h"
#include "utils.h"
#include "pathfinding.h"

#include <time.h>
#include <string.h>
#include <ctype.h> // tolower
#include <ncurses.h>

void kill_player( char map[MAP_HSIZE][MAP_WSIZE], gps player ){
  // map[ player.y ][ player.x ] = BLOOD;
  // timespec req;
  //   req.tv_sec = 0;
  //   req.tv_nsec = 400000000;

  // int i;
  // for( i = 1; i < 3; i++ ){
  //   change_chmap( map, player.y    , player.x - i, BLOOD, 0    );  //   +
  //   change_chmap( map, player.y    , player.x + i, BLOOD, 0    );  //  +Z+
  //   change_chmap( map, player.y - i, player.x    , BLOOD, 0    );  //   + 
  //   change_chmap( map, player.y + i, player.x    , BLOOD, 0    );
  //   change_chmap( map, player.y - i, player.x - i, BLOOD, DIRT );  //  + +
  //   change_chmap( map, player.y - i, player.x + i, BLOOD, DIRT );  //   Z
  //   change_chmap( map, player.y + i, player.x - i, BLOOD, DIRT );  //  + +
  //   change_chmap( map, player.y + i, player.x + i, BLOOD, DIRT );
  //   draw_map( map, MAP_Y, MAP_X );
  //   refresh();
  //   nanosleep( &req, 0 );
  // }

  // flushinp();
}

int move_obj( char map[MAP_HSIZE][MAP_WSIZE], int obj, int y, int x, int move ){
  int iy = y + dy[ move ];
  int ix = x + dx[ move ];
  if( ix >= 0 && iy >= 0 && ix < MAP_WSIZE && iy <  MAP_HSIZE ){
    char tmp_obj;
    tmp_obj = map[ iy ][ ix ];

    if( tmp_obj == EMPTY ){
      switch( move ){
      case UP   : map[ y ][ x ] = EMPTY; y--; break;
      case DOWN : map[ y ][ x ] = EMPTY; y++; break;
      case LEFT : map[ y ][ x ] = EMPTY; x--; break;
      case RIGHT: map[ y ][ x ] = EMPTY; x++; break;
      }
      map[ y ][ x ] = obj;
      return 1;
    } else return 0;
 } else return 0;
}

int move_player( char map[MAP_HSIZE][MAP_WSIZE], gps* player, int move ){
  int iy = player->y + dy[ move ];
  int ix = player->x + dx[ move ];
  if( ix < 0 || iy < 0 || ix >= MAP_WSIZE || iy >= MAP_HSIZE ) return 0;

  char obj = map[ iy ][ ix ];

  if( obj == MONSTER ){
    return -1;
  }

  if( obj == STONE || obj == BOMB ){
    if( move_obj( map, obj, iy, ix, move ) ){
      map[ player->y ][ player->x ] = EMPTY;
      player->y += dy[ move ];
      player->x += dx[ move ];
      map[ player->y ][ player->x ] = PLAYER;
      return 1;
    } else return 0;
  }

  if( obj == EMPTY || obj == DIRT || obj == DIAMOND || obj == MONEY || obj == BOMBPK ){
    map[ player->y ][ player->x ] = EMPTY;
    player->y += dy[ move ];
    player->x += dx[ move ];
    map[ player->y ][ player->x ] = PLAYER;
    return 1;
  } else return 0;
}

void draw_all( char map[MAP_HSIZE][MAP_WSIZE], char path_map[MAP_HSIZE][MAP_WSIZE],
               int delay ){
  if( delay ){
    int i, ii, count_m = 0;
    for( i = 0; i < MAP_HSIZE; i++ ) 
      for( ii = 0; ii < MAP_WSIZE; ii++ )
        if( map[i][ii] == MONSTER ) count_m++;

    timespec req;
    req.tv_sec = 0;
    req.tv_nsec = delay;
    nanosleep( &req, 0 );
    draw_map( map, MAP_Y, MAP_X );
    draw_map( path_map, MAP_Y, MAP_X + MAP_WSIZE );
    mvprintw( 0,  1, "monsters %3i", count_m );
    refresh();
  } else {
    int i, ii, count_m = 0;
    for( i = 0; i < MAP_HSIZE; i++ ) 
      for( ii = 0; ii < MAP_WSIZE; ii++ )
        if( map[i][ii] == MONSTER ) count_m++;

    draw_map( map, MAP_Y, MAP_X );
    draw_map( path_map, MAP_Y, MAP_X + MAP_WSIZE );
    mvprintw( 0,  1, "monsters %3i", count_m );
    refresh();
  }
}

int load_level( char map[MAP_HSIZE][MAP_WSIZE], int nlevel, gps* player ){
  if( nlevel >= 1 && nlevel <= 11 ){
    char file[30];
    sprintf( file, "data/levels/%02i", nlevel );
    if( !load_map( file, map ) ){
      msgbox( "no-load level" );
      return 0;
    }

    int i, ii;
    for(i = 0; i < MAP_HSIZE; i++)
      for(ii = 0; ii < MAP_WSIZE; ii++){
        if( map[i][ii] == PLAYER ){ 
          player->y = i;
          player->x = ii;
        }
      }

    return 1;
  } else return 0;
}

int kaboom( char map[MAP_HSIZE][MAP_WSIZE] ){
  timespec req;
  req.tv_sec = 0;
  req.tv_nsec = 50000000;
  int die = 0;
  int i, ii, move;
  int iy, ix;
  char obj;
  for(i = 0; i < MAP_HSIZE; i++)
    for(ii = 0; ii < MAP_WSIZE; ii++){
      if( map[i][ii] == BOMB ){
        map[i][ii] = EMPTY;
        for( move = 0; move < MAX_MV; move++ ){
          iy =  i + dy[ move ];
          ix = ii + dx[ move ];
          if( ix >= 0 && iy >= 0 && ix < MAP_WSIZE && iy <  MAP_HSIZE ){
            obj = map[iy][ix];
            if( obj == DIRT || obj == STONE || obj == MONSTER ) map[iy][ix] = EMPTY;
            if( obj == PLAYER ) die = 1;
          }
        }    // for move

        draw_map( map, MAP_Y, MAP_X );
        refresh();
        nanosleep( &req, 0 );
      }      // if  map == BOMB
    }        // for MAP_WSIZE

  flushinp();
  return die;
}

int do_the_monster_dance( char map[MAP_HSIZE][MAP_WSIZE], char path_map[MAP_HSIZE][MAP_WSIZE],
                          gps player ) {
  char IO_map[MAP_HSIZE][MAP_WSIZE];
  memset( IO_map, 1, sizeof(char) * MAP_HSIZE * MAP_WSIZE );
  char monster_map[MAP_HSIZE][MAP_WSIZE];
  memset( monster_map, 0, sizeof(char) * MAP_HSIZE * MAP_WSIZE );

  int iy, ix; 
  for( iy = 0; iy < MAP_HSIZE; iy++ ) 
    for( ix = 0; ix < MAP_WSIZE; ix++ ){
      if( map[iy][ix] == DIRT || map[iy][ix] == DIAMOND || map[iy][ix] == WALL ||
          map[iy][ix] == STONE || map[iy][ix] == BOMB )
        IO_map[iy][ix] = 0;
      if( map[iy][ix] == MONSTER )
        monster_map[iy][ix] = 1;
    }

  path_find( IO_map, path_map, player );

  gps next;
  for( iy = 0; iy < MAP_HSIZE; iy++ ) 
    for( ix = 0; ix < MAP_WSIZE; ix++ )
      if( monster_map[iy][ix] ){
        if( path_map[iy][ix] < 4 ){
          next.y = iy + dy[ path_map[iy][ix] ];
          next.x = ix + dx[ path_map[iy][ix] ];

          if( map[ next.y ][ next.x ] != MONSTER ){
            if( next.y == player.y && next.x == player.x  ){
            map[ iy     ][ ix     ] = EMPTY;
            } else {
            map[ iy     ][ ix     ] = EMPTY;
            map[ next.y ][ next.x ] = MONSTER;
            //if( next.y == player.y && next.x == player.x  ) return 1;
            }
          }
        }
      }

  return 0;
}

void help(){
  WINDOW* helpwin = newwin( 20, 50, (GLOBAL_HEIGHT - 20)/2, (GLOBAL_WIDTH - 50) / 2 );
  wbkgd( helpwin, COLOR_PAIR( YB ) );
  box( helpwin, 0, 0 );
  mvwaddstr( helpwin,  1, 1, "asx cjnchkjsd sdkjcnsd" ); 
  mvwaddstr( helpwin, 11, 1, "asx cjnchkjsd sdkjcnsd" ); 
  mvwaddstr( helpwin, 17, 1, "asx cjnchkjsd sdkjcnsd" ); 
  mvwaddstr( helpwin,  8, 1, "asx cjnchkjsd sdkjcnsd" ); 
  mvwaddstr( helpwin,  7, 1, "asx cjnchkjsd sdkjcnsd" ); 
  wrefresh( helpwin );
  nodelay( stdscr, FALSE );
  getch();
  nodelay( stdscr, TRUE );
  delwin( helpwin );
}

int game( int nlevel ){
  int die;
  gps player;
  int count = 0;
  int level = nlevel;
  char map[MAP_HSIZE][MAP_WSIZE];
  char path_map[MAP_HSIZE][MAP_WSIZE];
  memset( path_map, 4, sizeof(char) * MAP_HSIZE * MAP_WSIZE );

  erase();
  load_level( map, level, &player );
  draw_all( map, path_map, HZ );

  nodelay( stdscr, TRUE );

  int run = 1;
  while( run ){
    count++;
    die = 0;
    switch( tolower( getch() ) ){
    case KEY_UP   : if( move_player( map, &player, UP    ) == -1 ) die = 1; break;
    case KEY_DOWN : if( move_player( map, &player, DOWN  ) == -1 ) die = 1; break;
    case KEY_LEFT : if( move_player( map, &player, LEFT  ) == -1 ) die = 1; break;
    case KEY_RIGHT: if( move_player( map, &player, RIGHT ) == -1 ) die = 1; break;
    case 'p'      : nodelay( stdscr, FALSE ); getch(); nodelay( stdscr, TRUE )  ; break;
    case 'r'      : load_level( map, level, &player )                           ; break;
    case 'h'      : help()                                                      ; break;
    case 27       : run = false                                                 ; break;
    case 'q'      : if(msgbox("Quit game == 'q' ? yes : no ")== 'q') run = false; break;

    default       :                break;
    } // switch( ch )

    if( count >= 4 ){
      if( do_the_monster_dance( map, path_map, player ) ) die = 1;
      count = 0;
    }
    if( die ){
      kill_player(map, player );
      if( msgbox( "continue == 'q' ? no : yes " ) != 'q' ) return level;
      else                              return 0    ;
    }


  for( int iy = 0; iy < MAP_HSIZE; iy++ ) 
    for( int ix = 0; ix < MAP_WSIZE; ix++ ){
      if( map[iy][ix] == MONSTER ) path_map[iy][ix] = 4;
    }

    draw_all( map, path_map, HZ );
  }   // while( run )

  nodelay( stdscr, FALSE );
  return 0;
}

// I'm Sorry (But Your Princess Is In Another Castle)
