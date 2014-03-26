#include "common.h"
#include "file.h"

#include <time.h>
#include <string.h>
#include <ncurses.h>

void print_menu( WINDOW* menu_win, int opt, int glevel, int elevel ){
  wclear( menu_win );
  mvwprintw( menu_win, 1, 1, "level  %02d", glevel );
  mvwprintw( menu_win, 2, 1, "editor %02d", elevel );
  mvwaddstr( menu_win, 3, 1, "EXIT" );
  mvwchgat( menu_win, opt, 1, 10, A_NORMAL, YB, NULL);
  wrefresh( menu_win );
}

int menu(){
  // draw screen ---------->
  erase();
  nodelay( stdscr, FALSE );
  attrset( COLOR_PAIR( RB ) | A_BOLD );
  draw_file( stdscr, "data/htext", 1, 20 );
  attrset( COLOR_PAIR( RB ) );
  mvprintw( 4, 80 - strlen( VERSION ), "%s", VERSION );

  attrset( COLOR_PAIR( GB ) );
  draw_file( stdscr, "data/tdesc", 9, 42 );

  int i;
  attrset( COLOR_PAIR( CB ) );
  for( i = 0; i < MAP_WSIZE; i++) {
    mvaddch( 22, i, ':');
    mvaddch(  0, i, ':');
  }

  attrset( COLOR_PAIR( MB ) );
  mvprintw(  23,  0, "CAVEZ of PHEAR Copyright 2003-2007 Tom Rune Flo <tom@x86.no>");
  mvprintw(  23, 62, "(mod by nasciiboy)");
  mvprintw(  24,  0, "Distributed under the terms of the GPL license");

  refresh();
  timespec req;
  req.tv_sec = 0;
  req.tv_nsec = 10000000;

  attrset( A_NORMAL );
  for( i = -40; i < 0; i++ ){
    draw_file( stdscr, "data/spgraf", 1, i );
    refresh();
    nanosleep( &req, 0 );
  }

  attrset( A_NORMAL );
  flushinp();
  // <---------- draw screen

  WINDOW* menu_win;
  int ch = 0;
  int glevel = 1;
  int elevel = 1;
  int opt = 1;
  menu_win = newwin( 5, 15, 14, 47 );
  print_menu( menu_win, opt, glevel, elevel );

  while( ( ch = getch() ) != 10 ){
    switch( ch ){
    case KEY_UP:
      if( opt == 1 ) opt = 3;
      else             --opt;
      break;
    case KEY_DOWN:
      if( opt == 3 ) opt = 1;
      else             ++opt;
      break;
    case KEY_LEFT:
      if(      opt == 1 ) glevel -= glevel > 1 ? 1 : 0;
      else if( opt == 2 ) elevel -= elevel > 1 ? 1 : 0;
      break;
    case KEY_RIGHT:
      if(      opt == 1 ) glevel += glevel < 11 ? 1 : 0;
      else if( opt == 2 ) elevel += elevel < 11 ? 1 : 0;
      break;
    case 'q'      : return 0;
    case 27       : return 0;
    } // switch

    print_menu( menu_win, opt, glevel, elevel );
  }   // while

  if(      opt == 1 ) return glevel;
  else if( opt == 2 ) return -elevel;
  else                return 0;
}
