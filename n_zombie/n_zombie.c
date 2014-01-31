/**
 *  n_zombie -- ncurses game --
 *  Copyright (C) <2014>  <nasciiboy>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 **/

#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define H            25
#define W            80
#define T      15000000
#define WAVE        300
#define ZVEL        100
#define BSIZE         2
#define ZSIZE        30
#define FSIZE         3

#define CAT( fil, cont )   strcpy( &n_zombie[ fil ][ 0 ], #cont )

void colors(){
  int i, ii, ck = 1;
  for( i = 0; i < 8; i++ ){
    for( ii = 0; ii < 8; ii++ )
      init_pair( ck++, i, ii );
  }
}

enum CK {  BB = 1, BR, BG, BY, BX, BM, BC, BW,
               RB, RR, RG, RY, RX, RM, RC, RW,
               GB, GR, GG, GY, GX, GM, GC, GW,
               YB, YR, YG, YY, YX, YM, YC, YW,
               XB, XR, XG, XY, XX, XM, XC, XW,
               MB, MR, MG, MY, MX, MM, MC, MW,
               CB, CR, CG, CY, CX, CM, CC, CW,
               WB, WR, WG, WY, WX, WM, WC, WW, CKMAX };

struct bullet {
  int posY, posX;
  bool is_moving;
};

struct zombie {
  int posY, posX, move;
  bool is_moving;
};

struct fence {
  int tot;
};

int main(){
  initscr();
  start_color();
  colors();
  cbreak();
  noecho();
  curs_set( FALSE );
  keypad( stdscr, TRUE );
  srand( time( 0 ) );
  timespec req;
  req.tv_sec = 0;
  req.tv_nsec = T;

  if( LINES < H || COLS < W ){
    while( LINES < H || COLS < W ){
      erase();
      mvprintw( LINES/2, COLS/2 - 15, "LINES %d >= %d -- COLS %d >= %d",
                LINES, H, COLS, W );
      getch();
      refresh();
    }
    clear();
  }

  char n_zombie[ 25 ][ 83 ];
  CAT( 0,"                                                                                ");
  CAT( 1,"                                                                                ");
  CAT( 2,"                                                                                ");
  CAT( 3,"                                                                                ");
  CAT( 4,"   _|      _|                                          _|        _|             ");
  CAT( 5,"   _|_|    _|      _|_|_|_|    _|_|    _|_|_|  _|_|    _|_|_|          _|_|     ");
  CAT( 6,"   _|  _|  _|          _|    _|    _|  _|    _|    _|  _|    _|  _|  _|_|_|_|   ");
  CAT( 7,"   _|    _|_|        _|      _|    _|  _|    _|    _|  _|    _|  _|  _|         ");
  CAT( 8,"   _|      _|      _|_|_|_|    _|_|    _|    _|    _|  _|_|_|    _|    _|_|_|   ");
  CAT( 9,"                                                                                ");
  CAT(10,"                                                                                ");
  CAT(11,"            ,         _   _    o ___                                            ");
  CAT(12,"           /|   / o  | | | |   // (_)                                           ");
  CAT(13,"            |__/     | | | |    \__   _  _  _                                   ");
  CAT(14,"            | \   |  |/  |/     /    / |/ |/ |                                  ");
  CAT(15,"            |  \_/|_/|__/|__/   \___/  |  |  |_/                                ");
  CAT(16,"                                                                                ");
  CAT(17,"                                                                                ");
  CAT(18,"                                        ___,   _   _                            ");
  CAT(19,"                                       /   |  | | | |                           ");
  CAT(20,"                                      |    |  | | | |                           ");
  CAT(21,"                                      |    |  |/  |/                            ");
  CAT(22,"                                       \__/\_/|__/|__/                          ");
  CAT(23,"                                                                                ");
  CAT(24,"                                                                                ");

  char map[ H ][ W ];
  int posY = 11, posX = 5;
  bullet bullets[ BSIZE ];
  zombie zombies[ ZSIZE ];
  fence fences[ H - 4 ];
  int zombie_index = 0;
  int bullet_index = 0;
  int score = 0;
  int i, ii;

  attron( COLOR_PAIR( WG ) | A_BOLD );
  for( i = 0; i < 25; i++ ){
    for( ii = 1; ii < 81; ii++ )
      mvprintw( i, ii - 1, "%c", n_zombie[ i ][ ii ] );
  }
  attroff( COLOR_PAIR( WG ) | A_BOLD);
  refresh();
  getch();

  // init structs & map
  for( i = 0; i < BSIZE; i++ ) bullets[i].is_moving = false;
  for( i = 0; i < ZSIZE; i++ ) zombies[i].is_moving = false;

  for( i = 0; i < H - 4; i++ ) fences[i].tot = FSIZE;
  memset( map, ' ', H * W );
  for( i = 0; i < W; i++ ){
    map[  1 ][ i ] = '#';
    map[ H - 2 ][ i ] = '#';
  }

  bool exit = false;
  int cont = 0, key, wave = 1;
  int vel = ZVEL, zombie_total;
  while( !exit ){
    nanosleep( &req, 0 );
    timeout( 0 );

    if( LINES < H || COLS < W ){
      while( LINES < H || COLS < W ){
        erase();
        mvprintw( LINES/2, COLS/2 - 15, "LINES %d >= %d -- COLS %d >= %d",
                  LINES, H, COLS, W );
        getch();
        refresh();
      }
      clear();
    }

    // control
    key = getch();
    switch( key ){
    case KEY_UP:
      if( posY - 1 > 1 ){
        map[ posY ][ posX ] = map[ posY ][ posX + 1 ] = ' ';
        --posY;
      }
      break;
    case KEY_DOWN:
      if( posY + 1 < H - 2 ){
        map[ posY ][ posX ] = map[ posY ][ posX + 1 ] = ' ';
        ++posY;
      }
      break;
    case ' ':
      if( !bullets[ bullet_index ].is_moving ){
        bullets[ bullet_index ].is_moving = true;
        bullets[ bullet_index ].posY = posY;
        bullets[ bullet_index ].posX = posX + 2;
        if( ++bullet_index == BSIZE ) bullet_index = 0;
      }
      break;
    case 'q': endwin(); return 0;
    case 27:  endwin(); return 0;
    default:  break;
    } // switch

    // draw hero
    map[ posY ][ posX ] = 'P';
    map[ posY ][ posX + 1 ] = '=';

    // draw fences
    for( i = 0; i < H - 4; i++ )
      for( ii = 0; ii < fences[i].tot; ii++ )
        map[ i + 2 ][ ii + 11 ] = '>';

    // draw bullet
    for( i = 0; i < BSIZE; i++ ){
      if( bullets[i].is_moving ){
        map[ bullets[i].posY ][ bullets[i].posX ] = ' ';
        ++bullets[i].posX;
        if( bullets[ i ].posX == W  )
          bullets[ i ].is_moving = false;
        else map[ bullets[i].posY ][ bullets[i].posX ] = '*';
      }
    }

    // zombie control
    zombie_total = 0;
    for( i = 0; i < ZSIZE; ++i) {
      if( zombies[i].is_moving && zombies[i].move >= vel ){
        zombies[i].move = 0;
        map[ zombies[i].posY ][ zombies[i].posX ] = ' ';
        --zombies[i].posX;
        if( map[ zombies[i].posY ][ zombies[i].posX ] == '>' ){
          map[ zombies[i].posY ][ zombies[i].posX ] = '@';
          zombies[i].is_moving = false;
          fences[ zombies[i].posY - 2 ].tot--;
        } else
        map[ zombies[i].posY ][ zombies[i].posX ] = 'q';

        // reaches the end
        if( zombies[i].posX == 0 ) exit = true;
      }

      // collision with bullet
      if( zombies[i].is_moving ){
        for( ii = 0; ii < BSIZE; ++ii ) {
          if( bullets[ii].is_moving && bullets[ii].posY == zombies[i].posY )
            if( bullets[ii].posX  == zombies[i].posX ){
              bullets[ii].is_moving = false;
              zombies[i].is_moving = false;
              map[ bullets[ii].posY ][ bullets[ii].posX ] = ' ';
              map[ zombies[i].posY ][ zombies[i].posX ] = '@';
              ++score;
            }
        }
        ++zombies[i].move;
        ++zombie_total;
      }
    } // zonbies

    // new zombie
    if( cont > WAVE ){
      if( zombie_index < ZSIZE ){
        if( rand() % 50 == 47 ){
          if( !zombies[zombie_index].is_moving ){
            zombies[zombie_index].posX = W - 1;
            zombies[zombie_index].posY = ( rand() % ( H - 4 ) ) + 2;
            if( map[ zombies[zombie_index].posY ][ W - 1 ] != 'q' ){
              map[ zombies[zombie_index].posY ][ W - 1 ] = 'q';
              zombies[zombie_index].is_moving = true;
              zombies[zombie_index].move = 0;
              ++zombie_index;

            }
          }
        }
      } else {
        if( !zombie_total ){
          zombie_index = 0;
          cont = 0;
          wave++;
          if( vel ) vel -= ZVEL / 10;
        }
      }
    }

    // draw display
    for( i = 0; i < H; i++ )
      for( ii = 0; ii < W; ii++ )
        switch( map[i][ii] ){
        case ' ':
          attron( COLOR_PAIR( GG ) );
          mvaddch( i, ii, ' ' );
          attroff( COLOR_PAIR( GG ) );
          break;
        case '#':
          attron( COLOR_PAIR( BB ) );
          mvaddch( i, ii, '#' );
          attroff( COLOR_PAIR( BB ) );
          break;
        case '>':
          attron( COLOR_PAIR( WG ) | A_BOLD );
          mvaddch( i, ii, '>' );
          attroff( COLOR_PAIR( WG ) | A_BOLD );
          break;
        case 'P':
          attron( COLOR_PAIR( WG ) | A_BOLD );
          mvaddch( i, ii, 'P' );
          attroff( COLOR_PAIR( WG ) | A_BOLD );
          break;
        case '=':
          attron( COLOR_PAIR( WG ) | A_BOLD );
          mvaddch( i, ii, '=' );
          attroff( COLOR_PAIR( WG ) | A_BOLD );
          break;
        case '*':
          attron( COLOR_PAIR( YG ) | A_BOLD );
          mvaddch( i, ii, '*' );
          attroff( COLOR_PAIR( YG ) | A_BOLD );
          break;
        case '@':
          attron( COLOR_PAIR( RG ) | A_BOLD );
          mvaddch( i, ii, '@' );
          attroff( COLOR_PAIR( RG ) | A_BOLD );
          break;
        case 'q':
          attron( COLOR_PAIR( WG ) | A_BOLD );
          mvaddch( i, ii, 'q' );
          attroff( COLOR_PAIR( WG ) | A_BOLD );
          break;
        } // printf
    if( cont < WAVE ){
      attron( COLOR_PAIR( CB ) | A_BOLD );
      mvprintw( H / 2, W / 2 - 5, " wave %2d ", wave );
      attroff( COLOR_PAIR( CB ) | A_BOLD );
    }
    // mvprintw( 0, 0, "ztot = %2d cont = %5d index %2d", zombie_total, cont, zombie_index );
    cont++;
    refresh();
  }
  do {
    attron( COLOR_PAIR( RW ) | A_BOLD );
    mvprintw( H/2, W/4, "YOU LOSE" );
    attroff( COLOR_PAIR( RW ) | A_BOLD );
    attron( COLOR_PAIR( BG ) | A_BOLD );
    mvprintw( H/2 + 1, W/4, "   WAVE: %5d", wave );
    mvprintw( H/2 + 2, W/4, "ZOMBIES: %5d", score );
    attroff( COLOR_PAIR( BG ) | A_BOLD );
    refresh();
    key = getch();
  } while( key == ERR );

  endwin();
  return 0;
}
