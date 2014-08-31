#include <ncurses.h>
#include <stdio.h>

#include "game.h"
#include "editor.h"
#include "menu.h"
#include "utils.h"
#include "common.h"

int main( int argc, char** argv ){
  int ch;
  while( ( ch = getopt( argc, argv, "hve" ) ) != -1 ){
    switch( ch ){
    case 'h': puts( "nphear [-e] [-v] [-h]\n" ); break;
    case '?': puts( "-h help\n" ); break;
    case 'e': puts( "editor" ); break;
    case 'v': puts( VERSION ); break;
    }
  }

  start_curses();
  start_ck();
  erase();

  int run = 1;
  while( run ){
    int opt = menu();
    if( opt == 0 ) run = 0;
    else if( opt > 0){ 
      do {
        opt = game( opt );
      } while( opt );
    }
    else if( opt < 0){
        opt = editor( -( opt ) );
    }
  }

  end_curses();
  return 0;
}
