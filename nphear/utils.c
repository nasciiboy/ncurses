#include "utils.h"
#include "common.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

void start_curses(){
  initscr();
  noecho();
  curs_set( FALSE );
  cbreak();
  keypad( stdscr, TRUE );
}

void end_curses(){
  endwin();
}

void start_ck(){
  int i, ii, ck = 1;
  if( has_colors() == TRUE ){
    start_color();
    for( i = 0; i < 8; i++ )
      for( ii = 0; ii < 8; ii++ )
        init_pair( ck++, i, ii );
  } else {
    endwin();
    fprintf( stderr, "You terminal does not support color\n" );
    exit( 1 );
  }
}

void clrscr(){
  int i, ii, maxy, maxx;
  getmaxyx(stdscr, maxy, maxx);
  for( i = 0; i < maxy; i++)
    for( ii = 0; ii < maxx; ii++)
      mvaddch( i, ii, ' ');
}

/* para cambiar el color fg llame previamente a attron() con COLOR_BLACK como
   color bg, o bien a bkgd para cualquier combinacion de color */
void nasciiboy(){
#define CATNU( fil, msg ) strcpy( NU[fil], #msg )
#define NU_H  9
#define NU_W 17
  char NU[NU_H][NU_W + 3];
  CATNU( 0,"  ,           ,  ");
  CATNU( 1," /             \ ");
  CATNU( 2,"((__-^^-,-^^-__))"); 
  CATNU( 3," `-_---' `---_-' ");
  CATNU( 4,"  `--|o` 'o|--'  ");
  CATNU( 5,"     \  `  /     ");
  CATNU( 6,"      ): :(      ");
  CATNU( 7,"      :o_o:      ");
  CATNU( 8,"       "-"       ");

  int nu_y = ( LINES -  NU_H ) / 2;
  int nu_x  = ( COLS  - 18 ) / 2;

  int i, ii;
  timespec req;
  req.tv_sec  = 1;
  req.tv_nsec = 0;
  clear();

  // draw GNU ascii(nu)
  for( i = 0; i < NU_H; i++ )
    for( ii = 0; ii < NU_W; ii++ )
      mvaddch( nu_y + i, nu_x + ii, NU[ i ][ ii + 1 ] );
  refresh();
  nanosleep( &req, 0 );

  req.tv_sec  = 0;
  req.tv_nsec = 500000000;
  i = 0;
  // draw lines
  while( i < nu_y ){
    // mvchgat(             i, 0, -1, A_NORMAL, WW, NULL);
    // mvchgat( LINES - 1 - i, 0, -1, A_NORMAL, WW, NULL);
    for( ii = 0; ii < COLS; ii++ ){
      mvaddch(             i, ii, ' ' | A_REVERSE );
      mvaddch( LINES - 1 - i, ii, ' ' | A_REVERSE );
    }
    refresh();
    nanosleep( &req, 0 );
    i++;
  }

#define CATNB( fil, msg ) strcpy( NB[fil], #msg )
#define NB_H  5
#define NB_W 36
  char NB[NB_H][NB_W + 3];
  CATNB( 0,"                  _ _ ___           ");
  CATNB( 1," _ _  __ _ ___ __(_|_) _ ) ___ _  _ ");
  CATNB( 2,"| ' \/ _` (_-</ _| | | _ \/ _ \ || |");
  CATNB( 3,"|_||_\__,_/__/\__|_|_|___/\___/\_, |");
  CATNB( 4,"                               |__/ ");

  int nb_y = nu_y + 2;
  int nb_x = COLS - 1;
  req.tv_nsec = 75000000;
  // move & draw nasciiBoy & GNU ascii
  while( nu_x > 2 ){
    for( i = 0; i < NB_H; i++ )
      for( ii = 0; ii < NB_W; ii++ )
        if( nb_x + ii < COLS )
          mvaddch( nb_y + i, nb_x + ii, NB[ i ][ ii + 1 ] );
    for( i = 0; i < NU_H; i++ )
      for( ii = 0; ii < NU_W; ii++ )
        mvaddch( nu_y + i, nu_x + ii, NU[ i ][ ii + 1 ] );

    // move position
    nb_x--;
    if( nb_x < nu_x + NU_W + 4 ){
      mvaddch( nu_y + 2, nu_x + NU_W, ' ' );
      nu_x--;
    }

    refresh();
    nanosleep( &req, 0 );
  }

  req.tv_sec = 2;
  req.tv_nsec = 0;
  nanosleep( &req, 0 );
}

// requerimiento minimo de lineas y columnas
void min_size( int y, int x ){
  if( LINES < y || COLS < x ){
    while( LINES < y || COLS < x ){
      erase();
      mvprintw( LINES / 2, COLS / 3, "LINES %d >= %d -- COLS %d >= %d", LINES, y, COLS, x );
      refresh();
    }
    clear();
    refresh();
  }
}

int getopt( int argc, char * const argv[], const char *optchar ){
  static int opt = 1;

  if( argc < 1 ) return -1;
  if( argc > opt ){
    if( (argv[opt])[0] == '-' ){
      int i;
      for( i = 0; optchar[i] != '\0'; i++ )
        if( (argv[opt])[1] != '\0' )
          if( (argv[opt])[1] == optchar[i] && (argv[opt])[2] == '\0' ){
            opt++;
            return optchar[i];
          }
    }

    opt++;
    return '?';
  }

  return -1;
}

int msgbox( const char* msg ){
  if( msg ){
    int width = strlen( msg );
    WINDOW* msgwin = newwin( 3, width + 2, GLOBAL_HEIGHT / 2, ( GLOBAL_WIDTH - width ) / 2 );
    //    wbkgd( msgwin, A_REVERSE );
    box( msgwin, 0, 0 );
    mvwaddstr( msgwin, 1, 1, msg ); 
    wrefresh( msgwin );
    nodelay( stdscr, FALSE );
    int ch = getch();
    nodelay( stdscr, TRUE );
    wclear( msgwin );
    wrefresh( msgwin );
    delwin( msgwin );
    return ch;
  } else {
    char errmsg[] = "NO MSG";
    int width = strlen( errmsg );
    WINDOW* msgwin = newwin( 3, width + 2, GLOBAL_HEIGHT / 2, ( GLOBAL_WIDTH - width ) / 2 );
    wbkgd( msgwin, A_REVERSE );
    mvwaddstr( msgwin, 1, 1, errmsg ); 
    wrefresh( msgwin );
    nodelay( stdscr, FALSE );
    getch();
    nodelay( stdscr, TRUE );
    delwin( msgwin );
    return 0;
  }
}

void print_in_middle( WINDOW* win, int starty, const char* msg ){
  int width, x, y;

  if( win == 0 ) win = stdscr;
  if( starty == 0 ) y = getmaxy( win ) / 2;
  else y = starty;

  width = getmaxx( win );
  x = ( width - strlen( msg ) ) / 2;
  mvwprintw( win, y, x, "%s", msg );
}
