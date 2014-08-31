#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "screen.h"
#include "brick.h"

void init_screen(){
  if( !initscr() ){
    fprintf( stderr, "ERROR, initscr() return ERR\n" );
    exit( 1 );
  }
  if( !has_colors() ){
    endwin();
    printf( "ERROR, your terminal can't display colors.\n" );
    exit( 1 );
  }
  start_color();
  init_colors();
  clear();
  noecho();
  keypad( stdscr, TRUE );
  cbreak();
  curs_set( FALSE );
}

void restore_screen(){
  endwin();
}

void init_colors(){
  int i, ii, ck = 1;
  for( i = 0; i < 8; i++ ){
    for( ii = 0; ii < 8; ii++ )
      init_pair( ck++, i, ii );
  }
}

WINDOW* create_win( const unsigned int h, const unsigned int w,
                    const unsigned int y, const unsigned int x ){
  WINDOW* win = newwin( h, w, y, x );
  box( win, 0, 0 );
  refresh();
  wrefresh( win );
  return win;
}

WINDOW* create_wboard(){
  return create_win( WBOARD_HEIGHT, WBOARD_WIDTH, WBOARD_Y, WBOARD_X );
}

WINDOW* create_wpreview(){
  return create_win( WPREV_HEIGHT, WPREV_WIDTH, WPREV_Y, WPREV_X );
}

WINDOW* create_wscore(){
  return create_win( WSCORE_HEIGHT, WSCORE_WIDTH, WSCORE_Y, WSCORE_X );

}

void wait_start( WINDOW *win ){
  wattron( win, COLOR_PAIR( YB ) );
  mvwprintw( win, WBOARD_HEIGHT/2  - 1, (WBOARD_WIDTH - 12)/2 , "PRESS START!" );
  wattroff( win, COLOR_PAIR( YB ) );
  wrefresh( win );
  nodelay( stdscr, FALSE );
  getch();
  nodelay( stdscr, TRUE );
}

void ck_mvaddch( int ck, int y, int x, const char ch ){
  attron( COLOR_PAIR( ck ) );
  mvaddch( y, x, ch );
  attroff( COLOR_PAIR( ck ) );
}

void show_title(){
  ck_mvaddch( CB, 1,  8, 'N' );
  ck_mvaddch( MB, 1, 17, 'T' );
  ck_mvaddch( YB, 1, 26, 'R' );
  ck_mvaddch( WB, 1, 35, 'I' );
  ck_mvaddch( GB, 1, 44, 'S' );
}

void show_preview( WINDOW* win, const char brick ){
  clear_win( win );

  int color;
  switch( brick ){
  case 1:  color = WR; break;
  case 2:  color = WY; break;
  case 3:  color = WX; break;
  case 4:  color = WG; break;
  case 5:  color = WM; break;
  case 6:  color = WW; break;
  case 7:  color = WC; break;
  default: color =  0; break;
  }

  if(color != 0) wattron(win, COLOR_PAIR(color));
  int i, ii;
  for( i = 0; i < 4; i++ )
    for( ii = 0; ii < 4; ii++ )
      if( matrix_brick[brick - 1][i][ii] )
        mvwaddstr( win, i + 1, ii * 2 + 3, "  " );

  if(color != 0) wattroff(win, COLOR_PAIR(color));
  wrefresh( win );
}


void show_score( WINDOW *win, const int score, const int level, const int lines ){
  mvwprintw(win, 1, 2, "Level:");
  mvwprintw(win, 5, 2, "Score:");
  mvwprintw(win, 9, 2, "lines:");
  wattron(win, COLOR_PAIR( YB) );
  mvwprintw(win, 3, 2, "%7i", level );
  mvwprintw(win, 7, 2, "%7i", score );
  mvwprintw(win, 11, 2, "%7i", lines );
  wattroff(win, COLOR_PAIR(YB) );
  wrefresh(win);
}


void show_board( WINDOW* win, char board[BOARD_HEIGHT][BOARD_WIDTH] ){
  clear_win( win );
  int color;

  int i, ii;
  for( i = 0; i < BOARD_HEIGHT; i++ )
    for( ii = 0; ii < BOARD_WIDTH; ii++ ){
      if( board[i][ii] ){
        switch( board[i][ii] ){
        case 1:  color = WR; break;
        case 2:  color = WY; break;
        case 3:  color = WX; break;
        case 4:  color = WG; break;
        case 5:  color = WM; break;
        case 6:  color = WW; break;
        case 7:  color = WC; break;
        default: color =  0; break;
        }

        if( color != 0 ) wattron(win, COLOR_PAIR(color));
        mvwaddch( win, i + 1, ii + 1, ' ' );
        if( color != 0 ) wattroff(win, COLOR_PAIR(color));
      }
    }

  wrefresh( win );
}

void clear_win( WINDOW* win ){
  wclear( win );
  box( win, 0, 0 );
}


void get_brick( char brick, char mtx_brick[4][4] ){
  memcpy( mtx_brick, matrix_brick[ brick - 1 ], sizeof(char) * 4 * 4);
}

void rotate_brick( char mtx_brick[4][4], char board[BOARD_HEIGHT][BOARD_WIDTH],
                   char* y, char* x, bool dir ){
  char tmp_brick[4][4];
  int i, ii;
  if( dir ){
    for( i = 0; i < 4; i++)
      for( ii = 0; ii < 4; ii++)
        tmp_brick[i][ii] = mtx_brick[3 - ii][i];
  } else {
    for( i = 0; i < 4; i++)
      for( ii = 0; ii < 4; ii++)
        tmp_brick[i][ii] = mtx_brick[ii][3 - i];
  }

  adjust_brick( tmp_brick );

  if( !check_brick( tmp_brick, board, *y, *x ) )
  memcpy( mtx_brick, tmp_brick, sizeof(char) * 4 * 4);
}

void adjust_brick( char mtx_brick[4][4] ){
  char tmp_brick[4][4];
  memset( tmp_brick, 0, sizeof(char) * 4 * 4 );
  int i, ii;
  int ti = 0;
  int tii = 4;
  bool change;
  for( i = 0; i < 4; i++)
    for( ii = 0; ii < 4; ii++)
      if( mtx_brick[i][ii] )
        if( ii < tii ) tii = ii;

  for( i = 0; i < 4; i++){
    change = false;
    for( ii = 0; ii < 4; ii++)
      if( mtx_brick[i][ii] ){
        tmp_brick[ti][ii - tii] = mtx_brick[i][ii];
        change = true;
      }
    if( change ) ti++;
  }

  memcpy( mtx_brick, tmp_brick, sizeof(char) * 4 * 4);
}

void clear_brick( WINDOW* win, char mtx_brick[4][4], char y, char x ){
  int i, ii;
  for( i = 0; i < 4; i++ )
    for( ii = 0; ii < 4; ii++ )
      if( mtx_brick[i][ii] )
        if( y + i >= 0 )
          mvwaddstr( win, y + i + 1, x + ii * 2 + 1, "  " );
}

void draw_brick( WINDOW* win, char brick, char mtx_brick[4][4], char y, char x ){
  int color;
  switch( brick ){
  case 1:  color = WR; break;
  case 2:  color = WY; break;
  case 3:  color = WX; break;
  case 4:  color = WG; break;
  case 5:  color = WM; break;
  case 6:  color = WW; break;
  case 7:  color = WC; break;
  default: color =  0; break;
  }

  wattron(win, COLOR_PAIR(color));
  int i, ii;
  for( i = 0; i < 4; i++ )
    for( ii = 0; ii < 4; ii++ )
      if( mtx_brick[i][ii] )
        if( y + i >= 0 )
          mvwaddstr( win, y + i + 1, x + ii * 2 + 1, "  " );

  wattroff(win, COLOR_PAIR(color));
  wrefresh( win );
}

int check_brick( char mtx_brick[4][4], char board[BOARD_HEIGHT][BOARD_WIDTH],
                 char y, char x ){
  int i, ii;
  for( i = 0; i < 4; i++ ){
    for( ii = 0; ii < 4; ii++ ){
      if( mtx_brick[i][ii] ){
        // limites de campo
        if( y + i >= BOARD_HEIGHT ) return 1;
        if( x + ii * 2 >= BOARD_WIDTH ) return 1;
        if( x + ii * 2 < 0 ) return 1;

        // elementos dentro del campo
        if( y + i >= 0 )
          if( board[y + i][x + ii * 2] )
            return 1;
      }
    }
  }
  return 0;
}

void move_brick( WINDOW* win, char board[BOARD_HEIGHT][BOARD_WIDTH], char mtx_brick[4][4],
                char brick, char* y, char* x, char move ){
  clear_brick( win, mtx_brick, *y, *x );

  switch( move ){
  case MOVE:
    break;
  case LEFT:  if( !check_brick( mtx_brick, board, *y, (*x) - 2 ) ) *x -= 2;
    break;
  case RIGHT: if( !check_brick( mtx_brick, board, *y, (*x) + 2 ) ) *x += 2;
    break;
  case DOWN:  if( !check_brick( mtx_brick, board, (*y) + 1, *x ) ) *y += 1;
    break;
  case BOTTOM:
    while( !check_brick( mtx_brick, board, (*y) + 1, *x ) ) *y += 1;
    break;
  case ROTATE_L: rotate_brick( mtx_brick, board, y, x, 0 );
    break;
  case ROTATE_R: rotate_brick( mtx_brick, board, y, x, 1 );
    break;
  default: break;
  }

  draw_brick( win, brick, mtx_brick, *y, *x );
}
