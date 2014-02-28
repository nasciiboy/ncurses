#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include "nitris.h"
#include "game.h"
#include "screen.h"

void handle_signal( const int sig ){
  restore_screen();
  exit( 0 );
}

int main( int argc, char* argv[] ){
  signal(  SIGKILL, handle_signal );
  signal( SIGWINCH, handle_signal );
  signal(  SIGTERM, handle_signal );
  signal(   SIGINT, handle_signal );
  signal(  SIGQUIT, handle_signal );

  start_game();

  return 0;
}
