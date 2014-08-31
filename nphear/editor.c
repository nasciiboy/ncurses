#include "editor.h"
#include "utils.h"
#include "map.h"

#include <time.h>
#include <string.h>
#include <ctype.h>

void draw_chwin( WINDOW* win, int y, int x, int ch ){
  if( win == 0 ) win = stdscr;
  switch( ch ){
  case EMPTY  :mvwaddch( win, y, x, EMPTY  |COLOR_PAIR( WB )           );break;
  case DIRT   :mvwaddch( win, y, x, DIRT   |COLOR_PAIR( RB )| A_NORMAL );break;
  case STONE  :mvwaddch( win, y, x, STONE  |COLOR_PAIR( WB )| A_NORMAL );break;
  case DIAMOND:mvwaddch( win, y, x, DIAMOND|COLOR_PAIR( YB )| A_BOLD   );break;
  case WALL   :mvwaddch( win, y, x, WALL   |COLOR_PAIR( CB )| A_NORMAL );break;
  case MONEY  :mvwaddch( win, y, x, MONEY  |COLOR_PAIR( GB )| A_NORMAL );break;
  case PLAYER :mvwaddch( win, y, x, PLAYER |COLOR_PAIR( WB )| A_BOLD   );break;
  case BOMB   :mvwaddch( win, y, x, BOMB   |COLOR_PAIR( XB )| A_BOLD   );break;
  case BOMBPK :mvwaddch( win, y, x, BOMBPK |COLOR_PAIR( XB )| A_BOLD   );break;
  case MONSTER:mvwaddch( win, y, x, MONSTER|COLOR_PAIR( MB )| A_BOLD   );break;
  case BLOOD  :mvwaddch( win, y, x, BLOOD  |COLOR_PAIR( YB )| A_BOLD   );break;
  default     :mvwaddch( win, y, x, '?'                                );break;
  }
}

int save( char map[MAP_HSIZE][MAP_WSIZE], int nlevel ){
  char file[30];
  sprintf( file, "data/levels/%02i", nlevel );
  if( save_map( file, map ) ){
    curs_set( FALSE );
    msgbox( "Saved successfully!" );
    curs_set( TRUE );
    return 1;
  } else {
    curs_set( FALSE );
    msgbox( "ERROR: Unable to open file for writing!" );
    curs_set( TRUE );
    return 0;
  }
}

void draw_editor( char map[MAP_HSIZE][MAP_WSIZE], int delay ){
  if( delay ){
    timespec req;
    req.tv_sec = 0;
    req.tv_nsec = delay;
    nanosleep( &req, 0 );
    draw_map( map, MAP_Y, MAP_X );
    refresh();
  } else {
    draw_map( map, MAP_Y, MAP_X );
    refresh();
  }
}

int load_elevel( char map[MAP_HSIZE][MAP_WSIZE], int elevel, int* y, int* x ){
  if( elevel >= 1 && elevel <= 11 ){
    char file[30];
    sprintf( file, "data/levels/%02i", elevel );
    if( !load_map( file, map ) ){
      msgbox( "no-load" );
      return 0;
    } else {
      *y = MAP_HSIZE / 2;
      *x = MAP_WSIZE / 2;
      draw_editor( map, HZ );
      return 1;
    }
  } else return 0;
}

void ehelp(){
  curs_set( FALSE );
  WINDOW* helpwin = newwin( 20, 50, (GLOBAL_HEIGHT - 20)/2, (GLOBAL_WIDTH - 50) / 2 );
  wbkgd( helpwin, COLOR_PAIR( YB ) );
  box( helpwin, 0, 0 );
  print_in_middle( helpwin, 3, "MENU" );
  mvwaddstr( helpwin,  5, 5, "0  -  9   --> SET OBJ" );
  mvwaddstr( helpwin,  6, 5, "   l      --> DRAWN LOCK ON/OFF" );
  mvwaddstr( helpwin,  7, 5, "   h      --> HELP" );
  mvwaddstr( helpwin,  8, 5, "   s      --> SAVE" );
  mvwaddstr( helpwin,  9, 5, "   a      --> SET ALL CHARACTERS" );
  mvwaddstr( helpwin, 10, 5, "   r      --> RELOAD LEVEL" );
  mvwaddstr( helpwin, 11, 5, "q  - ESC  --> MENU" );
  wrefresh( helpwin );
  nodelay( stdscr, FALSE );
  getch();
  nodelay( stdscr, TRUE );
  delwin( helpwin );
  curs_set( TRUE );
}

void draw_status(int obj, int lock ){
  draw_chwin( stdscr, 0, 68, obj );

  if(lock == -1) {
    attrset(COLOR_PAIR(WG) | A_NORMAL);
    mvprintw(0, 77, "OFF");
  } else {
    attrset(COLOR_PAIR(WR) | A_BOLD);
    mvprintw(0, 77, "ON ");
  }

  attrset(A_NORMAL);
}

int editor( int elevel ){
  int lock = -1;
  char obj = EMPTY;
  int level = elevel;
  int curs_x, curs_y;
  char map[MAP_HSIZE][MAP_WSIZE];
  erase();

  attrset(COLOR_PAIR(GB));
  mvprintw(0, 5, "CAVEZ of PHEAR "VERSION );
  attrset(COLOR_PAIR(WB));
  mvprintw(0, 35, "EDITOR" );
  attrset(COLOR_PAIR(CB));
  mvprintw( 24, 2, "0(EMPTY) 1(#) 2(O) 3(*) 4(:) 5($) 6(@) 7(%) 8(M) 9(Z)" );
  attrset(COLOR_PAIR(WB));
  mvprintw( 24, 60, "h(HELP) q(QUIT)" );
  attrset(COLOR_PAIR(MB) | A_NORMAL);
  mvprintw(0, 60, "OBJECT:");
  attrset(COLOR_PAIR(COLOR_MAGENTA) | A_NORMAL);
  mvprintw(0, 71, "LOCK:");
  attrset(A_NORMAL);

  curs_set( TRUE );
  nodelay( stdscr, TRUE );

  load_elevel( map, level, &curs_y, &curs_x );

  // gps player;
  // gps old_player;

  // int i, ii;
  // for(i = 0; i < MAP_HSIZE; i++)
  //   for(ii = 0; ii < MAP_WSIZE; ii++)
  //     if( map[i][ii] == PLAYER ){
  //       old_player.y =  i;
  //       old_player.x = ii;
  //     }

  int run = 1;
  while( run ){
    switch( tolower( getch() ) ){

    case KEY_UP   : curs_y--      ; break;
    case KEY_DOWN : curs_y++      ; break;
    case KEY_LEFT : curs_x--      ; break;
    case KEY_RIGHT: curs_x++      ; break;

    case '0'      : obj = EMPTY   ; break;
    case '1'      : obj = DIRT    ; break;
    case '2'      : obj = STONE   ; break;
    case '3'      : obj = DIAMOND ; break;
    case '4'      : obj = WALL    ; break;
    case '5'      : obj = MONEY   ; break;
    case '6'      : obj = BOMB    ; break;
    case '7'      : obj = BOMBPK  ; break;
    case '8'      : obj = MONSTER ; break;
    case '9'      : obj = PLAYER  ; break;
    case 'l'      : lock *= -1    ; break;
    case 'h'      : ehelp()       ; break;
    case 27       : run = false   ; break;
    case 's'      : save( map, level )       ;           break;
    case 'r'      : load_elevel( map, level, &curs_y, &curs_x ); break;
    case 10       : map[curs_y][curs_x] = obj;           break;
    case ' '      : map[curs_y][curs_x] = obj; curs_x++; break;

    case 'a'      : 
      if( obj != PLAYER )
        memset( map, obj, sizeof(char) * MAP_HSIZE * MAP_WSIZE );
      break;
    case 'q'      :
      curs_set( FALSE );
      if( msgbox( "Quit editor == 'q' ? yes : no " ) == 'q' ) run = false;
      curs_set( TRUE );
      break;
    default       :                 break;
    } // switch( ch )

    if( curs_y >= MAP_HSIZE ) curs_y = 0;
    if( curs_y <  0         ) curs_y = MAP_HSIZE - 1;
    if( curs_x >= MAP_WSIZE ) curs_x = 0;
    if( curs_x <  0         ) curs_x = MAP_WSIZE - 1;

    if( lock == 1 ) map[curs_y][curs_x] = obj;

    draw_editor( map, HZ );
    draw_status( obj, lock );
    move( curs_y + MAP_Y, curs_x + MAP_X );
  }   // while( run )

  nodelay( stdscr, FALSE );
  curs_set( FALSE );
  return 0;
}
