#include "file.h"
#include "map.h"
#include "utils.h"

#include <time.h>
#include <string.h>
#include <ctype.h> // tolower
#include <ncurses.h>

int lives; 
int bombs;
int score;
int diamonds;
int kill;

void kill_player( char map[MAP_HSIZE][MAP_WSIZE], int y, int x ){
  lives--;
  bombs = 0;
  map[y][x] = BLOOD;
  timespec req;
    req.tv_sec = 0;
    req.tv_nsec = 400000000;


  int i;
  for( i = 1; i < 3; i++ ){
    change_chmap( map, y    , x - i, BLOOD, 0    );  //   +
    change_chmap( map, y    , x + i, BLOOD, 0    );  //  +Z+
    change_chmap( map, y - i, x    , BLOOD, 0    );  //   + 
    change_chmap( map, y + i, x    , BLOOD, 0    );
    change_chmap( map, y - i, x - i, BLOOD, DIRT );  //  + +
    change_chmap( map, y - i, x + i, BLOOD, DIRT );  //   Z
    change_chmap( map, y + i, x - i, BLOOD, DIRT );  //  + +
    change_chmap( map, y + i, x + i, BLOOD, DIRT );
    draw_map( map, MAP_Y, MAP_X );
    refresh();
    nanosleep( &req, 0 );
  }
  //  if( lives == 0 ) game_over();
  kill = 1;
}

int move_obj( char map[MAP_HSIZE][MAP_WSIZE], int obj, int y, int x, int move ){
  char tmp_obj;
  switch( move ){
  case UP   : tmp_obj = map[ y - 1 ][ x     ]; break;
  case DOWN : tmp_obj = map[ y + 1 ][ x     ]; break;
  case LEFT : tmp_obj = map[ y     ][ x - 1 ]; break;
  case RIGHT: tmp_obj = map[ y     ][ x + 1 ]; break;
  }

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
}

int move_player( char map[MAP_HSIZE][MAP_WSIZE], int* y, int* x, int move ){
  char obj;
  int ox, oy;
  switch( move ){
  case UP   : obj = map[ oy = *y - 1 ][ ox = *x     ]; break;
  case DOWN : obj = map[ oy = *y + 1 ][ ox = *x     ]; break;
  case LEFT : obj = map[ oy = *y     ][ ox = *x - 1 ]; break;
  case RIGHT: obj = map[ oy = *y     ][ ox = *x + 1 ]; break;
  }

  if( obj == MONSTER ){
    kill_player( map, *y, *x );
    return 0;
  }

  if( obj == STONE || obj == BOMB ){
    if( move_obj( map, obj, oy, ox, move ) ){
      switch( move ){
      case UP   : map[ *y ][ *x ] = EMPTY; (*y)--; break;
      case DOWN : map[ *y ][ *x ] = EMPTY; (*y)++; break;
      case LEFT : map[ *y ][ *x ] = EMPTY; (*x)--; break;
      case RIGHT: map[ *y ][ *x ] = EMPTY; (*x)++; break;
      }
      map[ *y ][ *x ] = PLAYER;
      return 1;
    } else return 0;
  }

  if( obj == EMPTY || obj == DIRT || obj == DIAMOND || obj == MONEY || obj == BOMBPK ){
    switch( move ){
    case UP   : map[ *y ][ *x ] = EMPTY; (*y)--; break;
    case DOWN : map[ *y ][ *x ] = EMPTY; (*y)++; break;
    case LEFT : map[ *y ][ *x ] = EMPTY; (*x)--; break;
    case RIGHT: map[ *y ][ *x ] = EMPTY; (*x)++; break;
    }
    if( obj == MONEY   )  score += POINTS_MONEY  ;
    if( obj == DIAMOND ){ score += POINTS_DIAMOND; diamonds--; }
    if( obj == BOMBPK  )  bombs += NUM_BOMBPK    ;

    map[ *y ][ *x ] = PLAYER;
    return 1;
  } else return 0;
}

int gravity( char map[MAP_HSIZE][MAP_WSIZE], int* new_bomb ){
  int i, ii;
  char current, down, down_l, down_r, m_down, m_down_l, m_down_r, left, right;
  for( i = MAP_HSIZE - 1; i >= 0; i-- )
    for( ii = 0; ii < MAP_WSIZE; ii++ ){
      if( i == MAP_HSIZE - 1 ){
        current   = map[i][ii    ];
        down      =              0;
        down_l    =              0;
        down_r    =              0;
        m_down    =              0;
        m_down_l  =              0;
        m_down_r  =              0;
        if( ii == 0 ){
          right   = map[i][ii + 1];
          left    =              0;
        } else if( ii == MAP_WSIZE - 1 ){
          left    = map[i][ii - 1];
          right   =              0;
        } else {
          left    = map[i][ii - 1];
          right   = map[i][ii + 1];
        }
      } else if( i == MAP_HSIZE - 2 ){
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
          && down == EMPTY ){
        if( *new_bomb ) *new_bomb = 0;
        else map[i][ii] = EMPTY;
        map[i + 1][ii] = current;
        if( m_down == PLAYER ) kill_player( map, i + 2, ii );
      } else if( ( current == STONE || current == DIAMOND || current == MONEY ||
                   current == BOMBPK || current == BOMB || current == BLOOD ) && 
                 ( down == STONE || down == DIAMOND || down == MONEY ||
                   down == BOMBPK || down == BOMB || down == BLOOD )
                 && down_l == EMPTY && left == EMPTY ){
        if( *new_bomb ) *new_bomb = 0;
        else map[i][ii] = EMPTY;
        map[i + 1][ii - 1] = current;
        if( m_down_l == PLAYER ) kill_player( map, i + 2, ii - 1 );
      } else if( ( current == STONE || current == DIAMOND || current == MONEY ||
                   current == BOMBPK || current == BOMB || current == BLOOD ) && 
                 ( down == STONE || down == DIAMOND || down == MONEY ||
                   down == BOMBPK || down == BOMB || down == BLOOD ) 
                 && down_r == EMPTY && right == EMPTY ){
        if( *new_bomb ) *new_bomb = 0;
        else map[i][ii] = EMPTY;
        map[i + 1][ii + 1] = current;
        if( m_down_r == PLAYER ) kill_player( map, i + 2, ii + 1 );
      }
    } // for( ii < MAP_WSIZE )

  return 1;
}

void draw_all( char map[MAP_HSIZE][MAP_WSIZE], int delay ){
  if( delay ){
    timespec req;
    req.tv_sec = 0;
    req.tv_nsec = delay;
    nanosleep( &req, 0 );
    draw_map( map, MAP_Y, MAP_X );
    mvprintw( 0,  1, "diamonds %2i", diamonds );
    mvprintw( 0, 15, "score %2i", score );
    mvprintw( 0, 30, "lives %2i", lives );
    mvprintw( 0, 45, "bombs %2i", bombs );
    refresh();
  } else {
    draw_map( map, MAP_Y, MAP_X );
    mvprintw( 0,  1, "diamonds %2i", diamonds );
    mvprintw( 0, 15, "score %2i", score );
    mvprintw( 0, 30, "lives %2i", lives );
    mvprintw( 0, 45, "bombs %2i", bombs );
    refresh();
  }
}

int load_level( char map[MAP_HSIZE][MAP_WSIZE], int nlevel, int* y, int* x ){
  kill = 0;
  diamonds = 0;
  if( nlevel >= 1 && nlevel <= 11 ){
    char file[30];
    sprintf( file, "data/levels/%02i", nlevel );
    if( !load_map( file, map ) ){
      mvprintw( 15, 42, "no-load" );
      return 0;
    }

    int i, ii;
    for(i = 0; i < MAP_HSIZE; i++)
      for(ii = 0; ii < MAP_WSIZE; ii++){
        if( map[i][ii] == DIAMOND ) diamonds++;
        if( map[i][ii] == PLAYER ){ 
          *y = i;
          *x = ii;
        }
      }

    draw_all( map, HZ );
    return 1;
  } else return 0;
}

int kaboom( char map[MAP_HSIZE][MAP_WSIZE] ){
  timespec req;
  req.tv_sec = 0;
  req.tv_nsec = 50000000;
  int die = 0;
  int i, ii;
  char ch;
  for(i = 0; i < MAP_HSIZE; i++)
    for(ii = 0; ii < MAP_WSIZE; ii++){
      ch = map[i][ii];
      if( ch == BOMB ){
        map[i][ii] = EMPTY;
        if( change_chmap( map, i    , ii - 1, EMPTY, STONE ) == PLAYER ) die = 1;
        if( change_chmap( map, i    , ii + 1, EMPTY, STONE ) == PLAYER ) die = 1;
        if( change_chmap( map, i + 1, ii    , EMPTY, STONE ) == PLAYER ) die = 1;
        if( change_chmap( map, i - 1, ii    , EMPTY, STONE ) == PLAYER ) die = 1;
        if( change_chmap( map, i + 1, ii - 1, EMPTY, STONE ) == PLAYER ) die = 1;
        if( change_chmap( map, i + 1, ii + 1, EMPTY, STONE ) == PLAYER ) die = 1;
        if( change_chmap( map, i - 1, ii - 1, EMPTY, STONE ) == PLAYER ) die = 1;
        if( change_chmap( map, i - 1, ii + 1, EMPTY, STONE ) == PLAYER ) die = 1;
        if( change_chmap( map, i    , ii - 1, EMPTY, DIRT  ) == PLAYER ) die = 1;
        if( change_chmap( map, i    , ii + 1, EMPTY, DIRT  ) == PLAYER ) die = 1;
        if( change_chmap( map, i + 1, ii    , EMPTY, DIRT  ) == PLAYER ) die = 1;
        if( change_chmap( map, i - 1, ii    , EMPTY, DIRT  ) == PLAYER ) die = 1;
        if( change_chmap( map, i + 1, ii - 1, EMPTY, DIRT  ) == PLAYER ) die = 1;
        if( change_chmap( map, i + 1, ii + 1, EMPTY, DIRT  ) == PLAYER ) die = 1;
        if( change_chmap( map, i - 1, ii - 1, EMPTY, DIRT  ) == PLAYER ) die = 1;
        if( change_chmap( map, i - 1, ii + 1, EMPTY, DIRT  ) == PLAYER ) die = 1;
        change_chmap( map, i    , ii - 1, EMPTY, MONSTER );
        change_chmap( map, i    , ii + 1, EMPTY, MONSTER );
        change_chmap( map, i + 1, ii    , EMPTY, MONSTER );
        change_chmap( map, i - 1, ii    , EMPTY, MONSTER );
        change_chmap( map, i + 1, ii - 1, EMPTY, MONSTER );
        change_chmap( map, i + 1, ii + 1, EMPTY, MONSTER );
        change_chmap( map, i - 1, ii - 1, EMPTY, MONSTER );
        change_chmap( map, i - 1, ii + 1, EMPTY, MONSTER );
        draw_map( map, MAP_Y, MAP_X );
        refresh();
        nanosleep( &req, 0 );
      }
    }
  flushinp();
  return die;
}

int do_the_monster_dance( char map[MAP_HSIZE][MAP_WSIZE], int y, int x ) {
  char tmp_map[MAP_HSIZE][MAP_WSIZE];
  memset( tmp_map, EMPTY, sizeof( char ) * MAP_HSIZE * MAP_WSIZE );
  int iy, ix;
  for( iy = MAP_HSIZE - 1; iy >= 0; iy-- )
    for( ix = MAP_WSIZE - 1; ix >= 0; ix-- )
      if( map[ iy ][ ix ] == MONSTER ){
        if( x > ix )
          if( (map[iy][ix+1] == EMPTY ) && (tmp_map[iy][ix+1] == EMPTY ) ) {
            map[iy][ix] = EMPTY;
            tmp_map[iy][ix+1] = MONSTER;
          }
        if( map[iy][ix+1] == PLAYER ) kill_player( map, y, x );
      }
  for( iy = 0; iy < MAP_HSIZE; iy++ )
    for( ix = 0; ix < MAP_WSIZE; ix++ )
      if( tmp_map[ iy ][ ix ] == MONSTER )
        map[ iy ][ ix ] = MONSTER;
  memset( tmp_map, EMPTY, sizeof( char ) * MAP_HSIZE * MAP_WSIZE );

  for( iy = MAP_HSIZE - 1; iy >= 0; iy-- )
    for( ix = MAP_WSIZE - 1; ix >= 0; ix-- )
      if( map[ iy ][ ix ] == MONSTER ){
        if( y > iy )
          if( (map[iy+1][ix] == EMPTY ) && (tmp_map[iy+1][ix] == EMPTY )) {
            map[iy][ix] = EMPTY;
            tmp_map[iy+1][ix] = MONSTER;
          }
        if( map[iy+1][ix] == PLAYER ) kill_player( map, y, x );
      }
  for( iy = 0; iy < MAP_HSIZE; iy++ )
    for( ix = 0; ix < MAP_WSIZE; ix++ )
      if( tmp_map[ iy ][ ix ] == MONSTER )
        map[ iy ][ ix ] = MONSTER;
  memset( tmp_map, EMPTY, sizeof( char ) * MAP_HSIZE * MAP_WSIZE );

  for( iy = 0; iy < MAP_HSIZE; iy++ )
    for( ix = 0; ix < MAP_WSIZE; ix++ )
      if( map[ iy ][ ix ] == MONSTER ){
        if(x < ix )
          if( (map[iy][ix-1] == EMPTY  ) && (tmp_map[iy][ix-1] == EMPTY  )){
            map[iy][ix] = EMPTY;
            tmp_map[iy][ix-1] = MONSTER;
          }
        if( map[iy][ix-1] == PLAYER ) kill_player( map, y, x );
      }
  for( iy = 0; iy < MAP_HSIZE; iy++ )
    for( ix = 0; ix < MAP_WSIZE; ix++ )
      if( tmp_map[ iy ][ ix ] == MONSTER )
        map[ iy ][ ix ] = MONSTER;
  memset( tmp_map, EMPTY, sizeof( char ) * MAP_HSIZE * MAP_WSIZE );

  for( iy = 0; iy < MAP_HSIZE; iy++ )
    for( ix = 0; ix < MAP_WSIZE; ix++ )
      if( map[ iy ][ ix ] == MONSTER ){
        if( y < iy )
          if( (map[iy-1][ix] == EMPTY ) && ( tmp_map[iy-1][ix] == EMPTY ) ){
            map[iy][ix] = EMPTY;
            tmp_map[iy-1][ix] = MONSTER;
          }
        if( map[iy-1][ix] == PLAYER ) kill_player( map, y, x );
      }
  for( iy = 0; iy < MAP_HSIZE; iy++ )
    for( ix = 0; ix < MAP_WSIZE; ix++ )
      if( tmp_map[ iy ][ ix ] == MONSTER )
        map[ iy ][ ix ] = MONSTER;

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
  erase();
  lives = 3;
  bombs = 0;
  score = 0;
  diamonds = 0;
  kill = 0;

  int new_bomb = 0;
  int level = nlevel;
  int player_x, player_y;
  int new_bomb_x, new_bomb_y;
  char map[MAP_HSIZE][MAP_WSIZE];
  load_level( map, level, &player_y, &player_x );

  int run = 1;
  int ch;
  nodelay( stdscr, TRUE );
  while( run ){
    ch = tolower( getch() );
    switch( ch ){
    case KEY_UP   : move_player(map, &player_y, &player_x, UP    ); break;
    case KEY_DOWN : move_player(map, &player_y, &player_x, DOWN  ); break;
    case KEY_LEFT : move_player(map, &player_y, &player_x, LEFT  ); break;
    case KEY_RIGHT: move_player(map, &player_y, &player_x, RIGHT ); break;

    case 'k'      : kill_player(map,  player_y,  player_x        ); break;
    case 'h'      : help()                                        ; break;
    case 'b'      : 
      if( bombs ){
        if( new_bomb == 0 ){
          bombs--;
          new_bomb = 1;
          new_bomb_y = player_y;
          new_bomb_x = player_x;
        } 
      } break;
    case 't'      : 
      if( new_bomb ){
        new_bomb = 0;
        kill_player(map, player_y, player_x );
      } else
        if( kaboom( map ) ) kill_player(map, player_y, player_x );
      break;

    case 27       :  run = false;  break;
    case 'q'      :
      if( msgbox( "Quit game == 'q' ? yes : no " ) == 'q' ) run = false; break;
    default       :                break;
    } // switch( ch )

    if( new_bomb ){
      if( player_y != new_bomb_y || player_x != new_bomb_x ){
        new_bomb = 0;
        map[new_bomb_y][new_bomb_x] = BOMB;
      }
    }

    if( lives == 0    ){
      if( msgbox( "continue" ) != 'q' ) return level;
      else                              return 0    ;
    }
    if( diamonds <= 0 ) load_level( map, ++level, &player_y, &player_x );
    if( kill ) load_level( map, level, &player_y, &player_x );
    do_the_monster_dance( map, player_y, player_x );
    gravity( map, &new_bomb );
    draw_all( map, HZ );
  }   // while( run )

  nodelay( stdscr, FALSE );
  return 0;
}

// I'm Sorry (But Your Princess Is In Another Castle)
