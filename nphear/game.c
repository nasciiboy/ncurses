#include "file.h"
#include "map.h"
#include "utils.h"
#include "pathfinding.h"

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h> // tolower
#include <ncurses.h>

void kill_player( char map[MAP_HSIZE][MAP_WSIZE], gps player ){
  map[ player.y ][ player.x ] = BLOOD;
  timespec req;
    req.tv_sec = 0;
    req.tv_nsec = 400000000;

  int i;
  for( i = 1; i < 3; i++ ){
    change_chmap( map, player.y    , player.x - i, BLOOD, 0    );  //   +
    change_chmap( map, player.y    , player.x + i, BLOOD, 0    );  //  +Z+
    change_chmap( map, player.y - i, player.x    , BLOOD, 0    );  //   + 
    change_chmap( map, player.y + i, player.x    , BLOOD, 0    );
    change_chmap( map, player.y - i, player.x - i, BLOOD, DIRT );  //  + +
    change_chmap( map, player.y - i, player.x + i, BLOOD, DIRT );  //   Z
    change_chmap( map, player.y + i, player.x - i, BLOOD, DIRT );  //  + +
    change_chmap( map, player.y + i, player.x + i, BLOOD, DIRT );
    draw_map( map, MAP_Y, MAP_X );
    refresh();
    nanosleep( &req, 0 );
  }

  flushinp();
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

int move_player( char map[MAP_HSIZE][MAP_WSIZE], gps* player, int move, LSDB* lsdb ){
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
    if( obj == MONEY   )  lsdb->score += POINTS_MONEY  ;
    if( obj == DIAMOND ){ lsdb->score += POINTS_DIAMOND; (lsdb->diamonds)--; }
    if( obj == BOMBPK  )  lsdb->bombs += NUM_BOMBPK    ;
    return 1;
  } else return 0;
}

int gravity( char map[MAP_HSIZE][MAP_WSIZE], int* new_bomb ){
  int i, ii, die = 0;
  char current, down, down_l, down_r, m_down, m_down_l, m_down_r, left, right;
  for( i = MAP_HSIZE - 2; i >= 0; i-- )
    for( ii = 0; ii < MAP_WSIZE; ii++ ){
      // if( i == MAP_HSIZE - 1 ){
      //   current   = map[i][ii    ];
      //   down      =              0;
      //   down_l    =              0;
      //   down_r    =              0;
      //   m_down    =              0;
      //   m_down_l  =              0;
      //   m_down_r  =              0;
      //   if( ii == 0 ){
      //     right   = map[i][ii + 1];
      //     left    =              0;
      //   } else 
      //     if( ii == MAP_WSIZE - 1 ){
      //     left    = map[i][ii - 1];
      //     right   =              0;
      //   } else {
      //     left    = map[i][ii - 1];
      //     right   = map[i][ii + 1];
      //   }
      // } else
      if( i == MAP_HSIZE - 2 ){
        current   = map[i    ][ii    ];
        down      = map[i + 1][ii    ];
        m_down    =                  0;
        m_down_l  =                  0;
        m_down_r  =                  0;
        if( ii == 0 ){
          right   = map[i    ][ii + 1];
          down_r  = map[i + 1][ii + 1];
          left    =                  0;
          down_l  =                  0;
        } else if( ii == MAP_WSIZE - 1 ){
          left    = map[i    ][ii - 1];
          down_l  = map[i + 1][ii - 1];
          right   =                  0;
          down_r  =                  0;
        } else {
          right   = map[i    ][ii + 1];
          left    = map[i    ][ii - 1];
          down_r  = map[i + 1][ii + 1];
          down_l  = map[i + 1][ii - 1];
        }
      } else {
        current    = map[i    ][ii    ];
        down       = map[i + 1][ii    ];
        m_down     = map[i + 2][ii    ];
        if( ii == 0 ){
          right    = map[i    ][ii + 1];
          down_r   = map[i + 1][ii + 1];
          m_down_r = map[i + 2][ii + 1];
          left     =                  0;
          down_l   =                  0;
          m_down_l =                  0;
        } else if( ii == MAP_WSIZE - 1 ){
          left     = map[i    ][ii - 1];
          down_l   = map[i + 1][ii - 1];
          m_down_l = map[i + 2][ii - 1];
          right    =                  0;
          down_r   =                  0;
          m_down_r =                  0;
        } else {
          right    = map[i    ][ii + 1];
          left     = map[i    ][ii - 1];
          down_r   = map[i + 1][ii + 1];
          down_l   = map[i + 1][ii - 1];
          m_down_l = map[i + 2][ii - 1];
          m_down_r = map[i + 2][ii + 1];
        }
      }

      if( *new_bomb && current == PLAYER ) current = BOMB;

      if( ( current == STONE || current == DIAMOND || current == MONEY ||
            current == BOMBPK || current == BOMB || current == BLOOD )
          && ( down == EMPTY || down == MONSTER ) ){
        if( *new_bomb ) *new_bomb = 0;
        else map[i][ii] = EMPTY;
        map[i + 1][ii] = current;
        if( m_down == PLAYER ) die = 1;
      } else if( ( current == STONE || current == DIAMOND || current == MONEY ||
                   current == BOMBPK || current == BOMB || current == BLOOD ) && 
                 ( down == STONE || down == DIAMOND || down == MONEY ||
                   down == BOMBPK || down == BOMB || down == BLOOD )
                 && down_l == EMPTY && left == EMPTY ){
        if( *new_bomb ) *new_bomb = 0;
        else map[i][ii] = EMPTY;
        map[i + 1][ii - 1] = current;
        if( m_down_l == PLAYER ) die = 1;
      } else if( ( current == STONE || current == DIAMOND || current == MONEY ||
                   current == BOMBPK || current == BOMB || current == BLOOD ) && 
                 ( down == STONE || down == DIAMOND || down == MONEY ||
                   down == BOMBPK || down == BOMB || down == BLOOD ) 
                 && down_r == EMPTY && right == EMPTY ){
        if( *new_bomb ) *new_bomb = 0;
        else map[i][ii] = EMPTY;
        map[i + 1][ii + 1] = current;
        if( m_down_r == PLAYER ) die = 1;
      }
    } // for( ii < MAP_WSIZE )

  return die;
}

void draw_all( char map[MAP_HSIZE][MAP_WSIZE], int delay, LSDB lsdb ){
  if( delay ){
    timespec req;
    req.tv_sec = 0;
    req.tv_nsec = delay;
    nanosleep( &req, 0 );
    draw_map( map, MAP_Y, MAP_X );
    mvprintw( 0,  1, "diamonds %2i", lsdb.diamonds );
    mvprintw( 0, 15, "score %2i"   , lsdb.score );
    mvprintw( 0, 30, "lives %2i"   , lsdb.lives );
    mvprintw( 0, 45, "bombs %2i"   , lsdb.bombs );
    refresh();
  } else {
    draw_map( map, MAP_Y, MAP_X );
    mvprintw( 0,  1, "diamonds %2i", lsdb.diamonds );
    mvprintw( 0, 15, "score %2i"   , lsdb.score );
    mvprintw( 0, 30, "lives %2i"   , lsdb.lives );
    mvprintw( 0, 45, "bombs %2i"   , lsdb.bombs );
    refresh();
  }
}

int load_level( char map[MAP_HSIZE][MAP_WSIZE], int nlevel, gps* player, int* diamonds ){
  *diamonds = 0;
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
        if( map[i][ii] == DIAMOND ) (*diamonds)++;
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

int do_the_monster_dance( char map[MAP_HSIZE][MAP_WSIZE], gps player ) {
  char IO_map[MAP_HSIZE][MAP_WSIZE];
  memset( IO_map, 1, sizeof(char) * MAP_HSIZE * MAP_WSIZE );
  char monster_map[MAP_HSIZE][MAP_WSIZE];
  memset( monster_map, 0, sizeof(char) * MAP_HSIZE * MAP_WSIZE );
  char path_map[MAP_HSIZE][MAP_WSIZE];

  int iy, ix; 
  for( iy = 0; iy < MAP_HSIZE; iy++ ) 
    for( ix = 0; ix < MAP_WSIZE; ix++ ){
      if( map[iy][ix] == DIRT || map[iy][ix] == DIAMOND || map[iy][ix] == WALL ||
          map[iy][ix] == STONE || map[iy][ix] == BOMBPK || map[iy][ix] == BOMB )
        IO_map[iy][ix] = 0;
      if( map[iy][ix] == MONSTER )
        monster_map[iy][ix] = 1;
    }

  path_find( IO_map, path_map, player );

  gps next;
  for( iy = 0; iy < MAP_HSIZE; iy++ ) 
    for( ix = 0; ix < MAP_WSIZE; ix++ )
      if( monster_map[iy][ix] ){
        // pathfinding
        if( path_map[iy][ix] < 4 ){
          next.y = iy + dy[ int( path_map[iy][ix] ) ];
          next.x = ix + dx[ int( path_map[iy][ix] ) ];

          if( map[ next.y ][ next.x ] != MONSTER ){
            if( next.y == player.y && next.x == player.x  ){
              return 1;
              // map[ iy     ][ ix     ] = EMPTY;
            } else {
            map[ iy     ][ ix     ] = EMPTY;
            map[ next.y ][ next.x ] = MONSTER;
            }
          }
        } else { // do the monster dance
          int dir = rand() % 4;

          next.y = iy + dy[dir];
          next.x = ix + dx[dir];
          if( next.x < 0 || next.y < 0 || next.x >= MAP_WSIZE || next.y >= MAP_HSIZE ){
            // out of limits
          } else {
            if( map[ next.y ][ next.x ] == EMPTY ){
              map[ iy     ][ ix     ]    = EMPTY;
              map[ next.y ][ next.x ]    = MONSTER;
            }
          }

        } // else
      }   // if( moster_map )


  return 0;
}

void help(){
  WINDOW* helpwin = newwin( 20, 50, (GLOBAL_HEIGHT - 20)/2, (GLOBAL_WIDTH - 50) / 2 );
  wbkgd( helpwin, COLOR_PAIR( YB ) );
  box( helpwin, 0, 0 );

  print_in_middle( helpwin, 3, "Cavez of Phear" );
  mvwaddstr( helpwin,  6, 5, "   arrows --> MOVE PLAYER" );
  mvwaddstr( helpwin,  7, 5, "   h      --> HELP" );
  mvwaddstr( helpwin,  8, 5, "   t      --> KABOOM" );
  mvwaddstr( helpwin,  9, 5, "   b      --> BOMB" );
  mvwaddstr( helpwin, 10, 5, "   k      --> harakiri" );
  mvwaddstr( helpwin, 11, 5, " q - ESC  --> MENU" );

  wrefresh( helpwin );
  nodelay( stdscr, FALSE );
  getch();
  nodelay( stdscr, TRUE );
  delwin( helpwin );
}

int game( int nlevel ){
  LSDB lsdb;
  lsdb.lives    = 3;
  lsdb.bombs    = 0;
  lsdb.score    = 0;
  lsdb.diamonds = 0;

  int die;
  gps player;
  int count = 0;
  int new_bomb = 0;
  int level = nlevel;
  int level_score = 0;
  int new_bomb_x, new_bomb_y;
  char map[MAP_HSIZE][MAP_WSIZE];

  erase();
  load_level( map, level, &player, &lsdb.diamonds );
  draw_all( map, HZ, lsdb );

  nodelay( stdscr, TRUE );

  int run = 1;
  while( run ){
    count++;
    die = 0;
    switch( tolower( getch() ) ){
    case KEY_UP   : if( move_player( map, &player, UP   , &lsdb) == -1 ) die = 1; break;
    case KEY_DOWN : if( move_player( map, &player, DOWN , &lsdb) == -1 ) die = 1; break;
    case KEY_LEFT : if( move_player( map, &player, LEFT , &lsdb) == -1 ) die = 1; break;
    case KEY_RIGHT: if( move_player( map, &player, RIGHT, &lsdb) == -1 ) die = 1; break;
    case 'k'      :                                                      die = 1; break;
    case 'h'      : help()                                                      ; break;
    case 27       : run = false                                                 ; break;
    case 'q'      : if(msgbox("Quit game == 'q' ? yes : no ")== 'q') run = false; break;
    case 'b'      : 
      if( lsdb.bombs ){
        if( new_bomb == 0 ){
          lsdb.bombs--;
          new_bomb = 1;
          new_bomb_y = player.y;
          new_bomb_x = player.x;
        } 
      } break;
    case 't'      : 
      if( new_bomb ){
        new_bomb = 0;
        die = 1;
      } else if( kaboom( map ) ) die = 1;
      break;
    default       :                break;
    } // switch( ch )

    if( new_bomb ){
      if( player.y != new_bomb_y || player.x != new_bomb_x ){
        new_bomb = 0;
        map[new_bomb_y][new_bomb_x] = BOMB;
      }
    }

    if( count >= 4 ){
      if( do_the_monster_dance( map, player ) ) die = 1;
      count = 0;
    }
    if( gravity( map, &new_bomb )             ) die = 1;
    if( die ){
      kill_player(map, player );
      lsdb.lives--;
      lsdb.bombs = 0;
      new_bomb = 0;
      if( lsdb.lives == 0 ){
        if( msgbox( "continue == 'q' ? no : yes " ) != 'q' ) return level;
        else                              return 0    ;
      }

      lsdb.score = level_score;
      load_level( map, level, &player, &lsdb.diamonds );
    }
    if( lsdb.diamonds <= 0 ){
      level_score = lsdb.score;
      load_level( map, ++level, &player, &lsdb.diamonds );
    }

    draw_all( map, HZ, lsdb );
  }   // while( run )

  nodelay( stdscr, FALSE );
  return 0;
}

// I'm Sorry (But Your Princess Is In Another Castle)
