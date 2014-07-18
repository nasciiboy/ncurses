#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#include "sprites.h"

#define HEIGHT 40
#define WIDTH  80
#define SECOND 1000000
#define DECSEC  100000
#define SECSGAME 300

#define ENTER   10
#define ESC     27

#define D        2 
#define d        1
#define F        0
#define e       -1
#define E       -2

#define SIZE sizeof( chtype ) * 128 * 256

enum state { MENU, QUIT, SPLASH, GAME, WIN, DEAD, WINPOINT, GAMEOVER, GAMEWIN };
enum state screenGame;

enum ckcol { MARIO, SCORE, PISO, CLOUD, MOUNT, BLOCK, CASTLE, MENUD, LOGO, HOLE, COIN, GOMPA, FLAG, GOV, DEFAULT };

const int sizeFase = 3000;
chtype screen[ 128 ][ 256 ];
chtype fase[HEIGHT][sizeFase];

time_t timeIni;
time_t timeIniGame;
time_t timeGame;

clock_t tempoN;

int lps;
int fps;
int qs = 1;
bool next;

bool sound;
bool color;
int score;
int topScore = 777;
int coins;
int cameraX = 0;
int piso = 37;
int gravity = 1;

typedef struct {
  int size;
  int height;
  int width;
  int initY;
  int initX;
  int widthMid;
} attr_screen;

attr_screen ats;

typedef struct {
  int y, x;
  int ani;
  int sprite;
  int side;
  int height;
  int width;
  int velY;
  int velX;
  int dead;
  int lifes;
} attr_gamer;

attr_gamer gamer;

///////////////////// COLOR - PAIR /////////////////////////////////////////
enum CK {  BB = 1, BR, BG, BY, BX, BM, BC, BW,              // BLACK    B //
               RB, RR, RG, RY, RX, RM, RC, RW,              // RED      R //
               GB, GR, GG, GY, GX, GM, GC, GW,              // GREEN    G //
               YB, YR, YG, YY, YX, YM, YC, YW,              // YELLOW   Y //
               XB, XR, XG, XY, XX, XM, XC, XW,              // BLUE     X //
               MB, MR, MG, MY, MX, MM, MC, MW,              // MAGENTA  M //
               CB, CR, CG, CY, CX, CM, CC, CW,              // CYAN     C //
               WB, WR, WG, WY, WX, WM, WC, WW, CKMAX };     // WHITE    W //
////////////////////////////////////////////////////////////////////////////

int enemyX      [ 11 ];
int enemyPathIni[ 11 ];
int enemyPathFim[ 11 ];
int coin        [ 45 ];
int block       [ 30 ];
int cloud       [ 17 ];
int hole        [  8 ];
int mount       [ 22 ];

int enemyVelocX [ 10 ];
int enemySprite [ 10 ];
int aniEnemy    [ 10 ];
int enemyDead   [ 10 ];

int coinSprite  [ 44 ];


int cloudY = 2;
int enemyY = 27;
int coinY  = 9;
int blockY = 8;
int mountY = 30;
const int flagX = 2883;
int flagY;

int atblock = 0;
int atcoin = 0;

void SLEEP( int sec, int nsec ){
  timespec req;
  req.tv_sec  =  sec;
  req.tv_nsec = nsec;
  nanosleep( &req, 0 );
}

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
  getmaxyx( stdscr, maxy, maxx);
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
  clear();

  // draw GNU ascii(nu)
  for( i = 0; i < NU_H; i++ )
    for( ii = 0; ii < NU_W; ii++ )
      mvaddch( nu_y + i, nu_x + ii, NU[ i ][ ii + 1 ] );
  refresh();
  SLEEP( 1, 0 );

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
    SLEEP( 0, 500000000 );
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
    SLEEP( 0, 75000000 );
  }

  SLEEP( 2, 0 );
  flushinp();
}

void screencpy( int y, int x, const char *str, int draw ){
  int i;
  chtype ch;
  for( i = 0; ( ch = str[ i ] ) != '\0'; i++ ){
    if( y < 0 || x < 0  || y >= ats.height || x + i >= ats.width ){
    } else {
      switch( draw ){
      case MARIO:
        switch( ch ){
        case 'X':   screen[ y ][ x + i ] = ch | COLOR_PAIR( XX ); break;
        case ':':   screen[ y ][ x + i ] = ch | COLOR_PAIR( YY ); break;
        case ' ':   screen[ y ][ x + i ] = ch | COLOR_PAIR( GG ); break;
        case 'M':   screen[ y ][ x + i ] = ch | COLOR_PAIR( RX ); break;
        case '-':                                                 break;
        }
        break;
      case MENUD:
        switch( ch ){
        case 'X':   screen[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case ':':   screen[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case ' ':                                                 break;
        default :   screen[ y ][ x + i ] = ch | COLOR_PAIR( WC ); break;
        }
        break;
      case LOGO:
        switch( ch ){
        case '@':   screen[ y ][ x + i ] = ch | COLOR_PAIR( WR ); break;
        case ' ':   screen[ y ][ x + i ] = ch | COLOR_PAIR( RR ); break;
        case 'X':   screen[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case ':':   screen[ y ][ x + i ] = ch | COLOR_PAIR( XX ); break;
        }
        break;
      case COIN:
        switch( ch ){
        case '@':   screen[ y ][ x + i ] = ch | COLOR_PAIR( YC ) | A_BOLD ; break;
        case ' ':   screen[ y ][ x + i ] = ch | COLOR_PAIR( CC ); break;
        case '-':                                                 break;
        }
        break;
      case BLOCK:
        switch( ch ){
        case 'X':   screen[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case ':':   screen[ y ][ x + i ] = ch | COLOR_PAIR( BB ); break;
        case ' ':   screen[ y ][ x + i ] = ch | COLOR_PAIR( CC ); break;
        }
        break;
      case HOLE:
        switch( ch ){
        case 'X':   screen[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case ' ':   screen[ y ][ x + i ] = ch | COLOR_PAIR( CC ); break;
        }
        break;
      case GOMPA:
        switch( ch ){
        case 'X':   screen[ y ][ x + i ] = ch | COLOR_PAIR( RB ); break;
        case ' ':   screen[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case ':':   screen[ y ][ x + i ] = ch | COLOR_PAIR( WY ); break;
        case '>':   screen[ y ][ x + i ] = ch | COLOR_PAIR( RW ); break;
        case '<':   screen[ y ][ x + i ] = ch | COLOR_PAIR( RW ); break;
        case '-':                                                 break;
        }
        break;
      case FLAG:
        switch( ch ){
        case 'X':   screen[ y ][ x + i ] = ch | COLOR_PAIR( GG ); break;
        case ' ':   screen[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case '-':                                                 break;
        default :   screen[ y ][ x + i ] = ch | COLOR_PAIR( BW ); break;
        }
        break;
      case GOV:
        switch( ch ){
        case 'X':   screen[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case ' ':   screen[ y ][ x + i ] = ch | COLOR_PAIR( CC ); break;
        case '-':                                                 break;
        default :   screen[ y ][ x + i ] = ch | COLOR_PAIR( WC ); break;
        }
        break;
      case SCORE:   screen[ y ][ x + i ] = ch | COLOR_PAIR( WC ); break;
      case DEFAULT: screen[ y ][ x + i ] = ch | COLOR_PAIR( WC ); break;
      default:      screen[ y ][ x + i ] = ch                   ; break;    
      }
    }
  }
}

void facecpy( int y, int x, const char *str, int draw ){
  int i;
  chtype ch;
  for( i = 0; ( ch = str[ i ] ) != '\0'; i++ ){
    if( y < 0 || x < 0  || y >= HEIGHT || x + i >= sizeFase ){
    } else {
      switch( draw ){
      case MARIO:
        switch( ch ){
        case 'X':   fase[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case ':':   fase[ y ][ x + i ] = ch | COLOR_PAIR( XX ); break;
        case ' ':                                                 break;
        }
        break;
      case PISO:
        switch( ch ){
        case 'X':   fase[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case ':':   fase[ y ][ x + i ] = ch | COLOR_PAIR( GG ); break;
        }
        break;
      case CLOUD:
        switch( ch ){
        case 'X':   fase[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case ':':   fase[ y ][ x + i ] = ch | COLOR_PAIR( BB ); break;
        case ' ':   fase[ y ][ x + i ] = ch | COLOR_PAIR( CC ); break;
        case '-':                                               break;
        }
        break;
      case MOUNT:
        switch( ch ){
        case 'X':   fase[ y ][ x + i ] = ch | COLOR_PAIR( BB ); break;
        case ':':   fase[ y ][ x + i ] = ch | COLOR_PAIR( GB ); break;
        case ' ':   fase[ y ][ x + i ] = ch | COLOR_PAIR( GG ); break;
        case '-':                                               break;
        }
        break;
      case BLOCK:
        switch( ch ){
        case 'X':   fase[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case ':':   fase[ y ][ x + i ] = ch | COLOR_PAIR( BB ); break;
        case ' ':   fase[ y ][ x + i ] = ch | COLOR_PAIR( CC ); break;
        }
        break;
      case CASTLE:
        switch( ch ){
        case ' ':   fase[ y ][ x + i ] = ch | COLOR_PAIR( BB ); break;
        case '_':   fase[ y ][ x + i ] = ch | COLOR_PAIR( BC ); break;
        case '|':   fase[ y ][ x + i ] = ch | COLOR_PAIR( BC ); break;
        case '@':   fase[ y ][ x + i ] = ch | COLOR_PAIR( RG ); break;
        case 'X':   fase[ y ][ x + i ] = ch | COLOR_PAIR( WW ); break;
        case ':':   fase[ y ][ x + i ] = ch | COLOR_PAIR( MM ); break;
        case '-':                                               break;
        }
        break;
      case SCORE:   fase[ y ][ x + i ] = ch | COLOR_PAIR( WC ); break;
      case DEFAULT: fase[ y ][ x + i ] = ch                   ; break;
      default:      fase[ y ][ x + i ] = ch                   ; break;    
      }
    }
  }
}

void screenset( chtype ch ){
  int i, ii;
  for( i = 0; i < ats.height; i++ )
    for( ii = 0; ii < ats.width; ii++ )
      screen[ i ][ ii ] = ch;
}

void faseset( chtype ch ){
  int i, ii;
  for( i = 0; i < HEIGHT; i++ )
    for( ii = 0; ii < sizeFase; ii++ )
      fase[ i ][ ii ] = ch;
}

void ResizeScreen(){
  ats.widthMid = ats.width / 2;
  ats.initY = ats.height - HEIGHT;
  ats.initX = ( ats.width - WIDTH ) / 2;
  ats.size = ats.height * ats.width;

  clear();
}

void TerminalSize(){
  if( LINES < HEIGHT || COLS < WIDTH ){
    end_curses();
    fprintf( stderr,  
             "MÍNIMUM SIZE %3d LINES x %3d COLUMNS\n"
             "NOW          %3d LINES x %3d COLUMNS\n",
             HEIGHT, WIDTH, LINES, COLS );
    exit( 1 );
  } else {
    ats.height = LINES;
    ats.width  = COLS;
    ResizeScreen();
  }
}


void LoadMenu(){
  screenGame= MENU;
}

void GPL(){
  int y = ( ats.height - 17 ) / 2;
  int x = ( ats.width  - 69 ) / 2;
  mvprintw( y++, x, "Copyright (C) 2012 Doriedson Alves Galdino de Oliveira               " );
  mvprintw( y++, x, "                   Thiago Andre Silva                                " );
  mvprintw( y++, x, "                   Vitor Augusto Andrioli                            " );
  mvprintw( y++, x, "              2014 nasciiBoy                                         " );
  mvprintw( y++, x, "                                                                     " );
  mvprintw( y++, x, "This program is free software: you can redistribute it and/or modify " );
  mvprintw( y++, x, "it under the terms of the GNU General Public License as published by " );
  mvprintw( y++, x, "the Free Software Foundation, either version 3 of the License, or    " );
  mvprintw( y++, x, "(at your option) any later version.                                  " );
  mvprintw( y++, x, "                                                                     " );
  mvprintw( y++, x, "This program is distributed in the hope that it will be useful,      " );
  mvprintw( y++, x, "but WITHOUT ANY WARRANTY; without even the implied warranty of       " );
  mvprintw( y++, x, "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        " );
  mvprintw( y++, x, "GNU General Public License for more details.                         " );
  mvprintw( y++, x, "                                                                     " );
  mvprintw( y++, x, "You should have received a copy of the GNU General Public License    " );
  mvprintw( y++, x, "along with this program.  If not, see <http://www.gnu.org/licenses/>." );
  refresh();

  while( getch() == ERR );
  clear();
}

int date(){
  return clock();
}


int SECONDS(){
  return time( 0 ) - timeIni ;
}


void Score( int x ){
  score += x;
  if( score >= topScore ) topScore = score;
}

void DrawScore(){
  char info[81];
  sprintf( info, &scoreTitle[ 0 ][ 0 ] );
  screencpy( ats.initY, ats.initX, info, SCORE );
  sprintf( info, "%s%06d%s%02d%s%03ld%s", "     ", score, "                 @x", coins, "              1-1                 ", timeGame, "           " );
  screencpy( ats.initY + 1, ats.initX, info, SCORE );
  sprintf( info, "Color %3s - Sound %3s - LPS %3d - FPS %3d", color ? "yes" : "no", sound ? "yes" : "no", lps, fps );
  screencpy( ats.height - 1, 0, info, SCORE );
}

void MontaCanvas(){
  if( gamer.x < ats.widthMid ){
    cameraX=0;
  } else  {
    if( gamer.x >= ( sizeFase - ats.widthMid ) ){
      cameraX = sizeFase - ats.width;
    } else {
      cameraX= gamer.x - ats.widthMid;
    }
  }

  int _k;
  for( _k=0; _k < HEIGHT; _k++ )
    memcpy( &screen[ _k + ats.initY ][ 0 ], &fase[ _k ][ cameraX ], ats.width  *  sizeof( chtype ) );
}

void MontaMenu(){
  screenset( ' ' | COLOR_PAIR(  CC ) ); 

  int i;
  for( i = 0; i < HEIGHT; i++ )
    screencpy( ats.initY + i, ats.initX, &_menu[ i ][ 0 ], MENUD );

  screencpy( ats.initY, ats.initX, &scoreTitle[ 0 ][ 0 ], SCORE );

  for( i = 0; i < 13; i++ )
    screencpy( ats.initY + 3 + i, ats.initX +  15, &logo[ i ][ 0 ], LOGO );

  char str[80];
  sprintf( str, "                                TOP - %010d", topScore );
  screencpy( ats.initY + 26, ats.initX, str, SCORE );
}


void DrawJogador(){
  int _l;
  for( _l = 0; _l < 16; _l++ ){

    int tmpmariosprite;
    if( gamer.side == D ){
      tmpmariosprite = gamer.sprite;
    } else if( gamer.side == E ){
      tmpmariosprite = gamer.sprite + 3;
    } else {
      tmpmariosprite = 6;
    }

    if( gamer.y + _l >= ats.height ){
    } else screencpy( gamer.y + _l, gamer.x - cameraX, &_mario[ tmpmariosprite ][ _l ][ 0 ], MARIO );
  }
}

void DrawBuraco(){
 int _k;
 for( _k=0; hole[ _k ] != -1; _k++ ){
   if( ( ( hole[_k] + 25 /* width*/ )  >= cameraX ) && ( hole[ _k] <= ( cameraX + ats.width ) ) ){
     int _l;
     for( _l=37; _l<40; _l++ ){
       screencpy( _l + ats.initY, hole[ _k ] - cameraX - 2, _buracoDraw, HOLE );
     }
   }
 }
}

void DrawCoin(){
  int _k;
  for( _k= 0; coin[ _k ] != -1; _k++ ){
    if( coin[ _k ] != 0 ){
      if( ( ( coin[_k] + 10 /* width*/ )  >= cameraX ) && ( coin[ _k] <= ( cameraX + ats.width ) ) ){
        coinSprite[ _k ]++;
        if( coinSprite[ _k ] == 6 ){
          coinSprite[ _k ] = 0;
        }
        int _tmpSp= coinSprite[ _k ]/2;
        int _l;
        for( _l=0; _l< 5; _l++ ){
          screencpy( _l + ats.initY + coinY, coin[ _k ] - cameraX, &_coinDraw[_tmpSp ][ _l ][0 ], COIN );

        if( ( coin[ _k ] < ( gamer.x + gamer.width ) ) && ( ( coin[ _k ] +  10) > gamer.x  ) )
          atcoin = _k; 
        }
      }
    }
  }
}

void DrawFlag(){
  if( flagX < ( cameraX + ats.width ) ){
    int _k;
    for( _k=0; _k<7; _k++ ){
      screencpy( _k + ats.initY + flagY, flagX - cameraX, &_flagDraw[ _k ][0 ], FLAG );
    }
  }
}

void DrawEnemy(){
  int _k;
  for( _k=0; enemyX[ _k ] != -1; _k++ ){
    if( ( ( enemyX[_k] + 19 ) >= cameraX ) && ( enemyX[_k] <= ( cameraX + ats.width ) ) && ( enemySprite[_k] < 6 ) ){
      int _l;
      for( _l=0; _l<10; _l++ ){
        screencpy( _l + ats.initY + enemyY, enemyX[ _k ] - cameraX, &_gompa[ enemySprite[ _k ] ][ _l ][0 ], GOMPA );
      }
    }
  }
}

void DrawBlock(){
  int _k;
  for( _k=0; block[  _k] != -1; _k++ ){
    if( block[ _k ] != -2 ){
      if( ( ( block[_k] + 15 ) >= cameraX ) && ( block[_k] <= ( cameraX + ats.width ) ) ){
        int _l = 0;
        screencpy( blockY + _l + ats.initY, block[ _k ] - cameraX, &blockDraw[ 0 ][ 0 ], BLOCK );

        for( _l = 1; _l < 6; _l++ ){
          screencpy( blockY + _l + ats.initY, block[ _k ] - cameraX, &blockDraw[ 1 ][ 0 ], BLOCK );
        }

        screencpy( blockY + _l + ats.initY, block[ _k ] - cameraX, &blockDraw[ 0 ][ 0 ], BLOCK );

        if( ( block[ _k ] < ( gamer.x + gamer.width ) ) && ( ( block[ _k ] +  15) > gamer.x  ) )
          atblock = _k;

      }
    }
  }
}

void Render(){
  switch( screenGame ){
  case GAME:
  case DEAD:
  case WIN:
  case WINPOINT:
    MontaCanvas();
    break;
  case MENU:
    MontaMenu();
    break;
  default: break;
  }

  switch( screenGame ){
  case SPLASH:
  case GAMEOVER:
  case GAMEWIN:
  case MENU:
    DrawScore();
    break;
  case GAME:
  case DEAD:
  case WIN:
  case WINPOINT:
    DrawBlock();
    DrawBuraco();
    DrawScore();
    DrawCoin();
    DrawEnemy();
    DrawFlag();
    DrawJogador();
    break;
  default: break;
  }


  int i, ii;
  for( i = 0; i < ats.height; i++ )
    for( ii = 0; ii < ats.width; ii++ )
      mvaddch( i, ii, screen[ i ][ ii ] );
  screenset( ' ' | COLOR_PAIR( CC ) );
  
  refresh();
}

void GameWin(){
  screenGame=GAMEWIN;
  int _y = 10;

  screenset( ' ' | COLOR_PAIR( CC ) );

  screencpy( _y++, ats.initX, "      XXXXX    XXXX   XXXXX    XXXX   XXXXX   XXXXXX  XX  XX   XXXX     XX      ", DEFAULT ); 
  screencpy( _y++, ats.initX, "      XX  XX  XX  XX  XX  XX  XX  XX  XX  XX  XX      XXX XX  XX        XX      ", DEFAULT ); 
  screencpy( _y++, ats.initX, "      XXXXX   XXXXXX  XXXXX   XXXXXX  XXXXX   XXXX    XX XXX   XXXX     XX      ", DEFAULT ); 
  screencpy( _y++, ats.initX, "      XX      XX  XX  XX  XX  XX  XX  XX  XX  XX      XX  XX      XX            ", DEFAULT );
  screencpy( _y++, ats.initX, "      XX      XX  XX  XX  XX  XX  XX  XXXXX   XXXXXX  XX  XX   XXXX     XX      ", DEFAULT );   
  screencpy( _y++, ats.initX, "                                                                                ", DEFAULT );
  screencpy( _y++, ats.initX, "                                                                                ", DEFAULT );
  screencpy( _y++, ats.initX, "                                                                                ", DEFAULT );
  screencpy( _y++, ats.initX, "                   TRABALHO EM SHELL SCRIPT                                     ", DEFAULT );
  screencpy( _y++, ats.initX, "                   FATEC CARAPICUIBA                                            ", DEFAULT );
  screencpy( _y++, ats.initX, "                   DISCIPLINA LSO (LABORATORIO DE SISTEMAS OPERACIONAIS)        ", DEFAULT );
  screencpy( _y++, ats.initX, "                   PROF. RUBENS                                                 ", DEFAULT );
  screencpy( _y++, ats.initX, "                   ALUNO DORIEDSON ALVES GALDINO DE OLIVEIRA                    ", DEFAULT );
  screencpy( _y++, ats.initX, "                   ALUNO VITOR AUGUSTO ANDRIOLI                                 ", DEFAULT );
  screencpy( _y++, ats.initX, "                   ALUNO THIAGO ANDRE SILVA                                     ", DEFAULT );
  screencpy( _y++, ats.initX, "                                                                                ", DEFAULT );
  screencpy( _y++, ats.initX, "                   mod & c/ncurses version by                                   ", DEFAULT );
  screencpy( _y++, ats.initX, "                     N A S C I I B O Y                                          ", DEFAULT );
  screencpy( _y++, ats.initX, "                                                                                ", DEFAULT );

  Render();
  SLEEP(  4, 0 );
  while( getch() == ERR );

  LoadMenu();
}

void GameOver(){
  screenGame=GAMEOVER;
  int y;

  screenset( ' ' | COLOR_PAIR( CC ) );
  for( y = 0; y<5; y++ ){
    screencpy( ats.initY + y + 15, ats.initX, &_gameOver[y][0], GOV );
  }

  Render();
  SLEEP( 4, 0 );
  LoadMenu();
}

void LoadFase(){
  int i = 0;
  enemyX[ i++ ] = 200;
  enemyX[ i++ ] = 350;
  enemyX[ i++ ] = 500;
  enemyX[ i++ ] = 650;
  enemyX[ i++ ] = 900;
  enemyX[ i++ ] = 1490;
  enemyX[ i++ ] = 1750;
  enemyX[ i++ ] = 2030;
  enemyX[ i++ ] = 2450;
  enemyX[ i++ ] = 2780;
  enemyX[ i++ ] = -1;

  i = 0;
  enemyPathIni[ i++ ] = 150;
  enemyPathIni[ i++ ] = 320;
  enemyPathIni[ i++ ] = 460;
  enemyPathIni[ i++ ] = 610;
  enemyPathIni[ i++ ] = 880;
  enemyPathIni[ i++ ] = 1480;
  enemyPathIni[ i++ ] = 1710;
  enemyPathIni[ i++ ] = 2000;
  enemyPathIni[ i++ ] = 2330;
  enemyPathIni[ i++ ] = 2700;
  enemyPathIni[ i++ ] = -1;

  i = 0;
  enemyPathFim[ i++ ] = 250;
  enemyPathFim[ i++ ] = 420;
  enemyPathFim[ i++ ] = 560;
  enemyPathFim[ i++ ] = 710;
  enemyPathFim[ i++ ] = 980;
  enemyPathFim[ i++ ] = 1580;
  enemyPathFim[ i++ ] = 1810;
  enemyPathFim[ i++ ] = 2100;
  enemyPathFim[ i++ ] = 2480;
  enemyPathFim[ i++ ] = 2800;
  enemyPathFim[ i++ ] = -1;

  i = 0;
  coin[ i++ ] = 80;
  coin[ i++ ] = 190;
  coin[ i++ ] = 210;
  coin[ i++ ] = 330;
  coin[ i++ ] = 350;
  coin[ i++ ] = 370;
  coin[ i++ ] = 390;
  coin[ i++ ] = 410;
  coin[ i++ ] = 430;
  coin[ i++ ] = 660;
  coin[ i++ ] = 800;
  coin[ i++ ] = 830;
  coin[ i++ ] = 860;
  coin[ i++ ] = 890;
  coin[ i++ ] = 1010;
  coin[ i++ ] = 1080;
  coin[ i++ ] = 1150;
  coin[ i++ ] = 1240;
  coin[ i++ ] = 1260;
  coin[ i++ ] = 1280;
  coin[ i++ ] = 1370;
  coin[ i++ ] = 1390;
  coin[ i++ ] = 1410;
  coin[ i++ ] = 1480;
  coin[ i++ ] = 1500;
  coin[ i++ ] = 1520;
  coin[ i++ ] = 1540;
  coin[ i++ ] = 1590;
  coin[ i++ ] = 1650;
  coin[ i++ ] = 1720;
  coin[ i++ ] = 1810;
  coin[ i++ ] = 1830;
  coin[ i++ ] = 1850;
  coin[ i++ ] = 1870;
  coin[ i++ ] = 1890;
  coin[ i++ ] = 2260;
  coin[ i++ ] = 2290;
  coin[ i++ ] = 2320;
  coin[ i++ ] = 2350;
  coin[ i++ ] = 2410;
  coin[ i++ ] = 2540;
  coin[ i++ ] = 2570;
  coin[ i++ ] = 2600;
  coin[ i++ ] = 2630;
  coin[ i++ ] = -1;

  i = 0;
  block[ i++ ] = 105;
  block[ i++ ] = 135;
  block[ i++ ] = 165;
  block[ i++ ] = 495;
  block[ i++ ] = 525;
  block[ i++ ] = 555;
  block[ i++ ] = 630;
  block[ i++ ] = 690;
  block[ i++ ] = 975;
  block[ i++ ] = 1035;
  block[ i++ ] = 1110;
  block[ i++ ] = 1305;
  block[ i++ ] = 1320;
  block[ i++ ] = 1335;
  block[ i++ ] = 1560;
  block[ i++ ] = 1620;
  block[ i++ ] = 1680;
  block[ i++ ] = 1785;
  block[ i++ ] = 1920;
  block[ i++ ] = 2010;
  block[ i++ ] = 2040;
  block[ i++ ] = 2070;
  block[ i++ ] = 2100;
  block[ i++ ] = 2130;
  block[ i++ ] = 2160;
  block[ i++ ] = 2385;
  block[ i++ ] = 2670;
  block[ i++ ] = 2760;
  block[ i++ ] = 2790;
  block[ i++ ] = -1;

  i = 0;
  cloud[ i++ ] = 30;
  cloud[ i++ ] = 220;
  cloud[ i++ ] = 290;
  cloud[ i++ ] = 450;
  cloud[ i++ ] = 580;
  cloud[ i++ ] = 750;
  cloud[ i++ ] = 930;
  cloud[ i++ ] = 1180;
  cloud[ i++ ] = 1430;
  cloud[ i++ ] = 1740;
  cloud[ i++ ] = 1950;
  cloud[ i++ ] = 2180;
  cloud[ i++ ] = 2450;
  cloud[ i++ ] = 2720;
  cloud[ i++ ] = 2830;
  cloud[ i++ ] = 2965;
  cloud[ i++ ] = -1;

  i = 0;
  hole[ i++ ] = 280;
  hole[ i++ ] = 580;
  hole[ i++ ] = 850;
  hole[ i++ ] = 1420;
  hole[ i++ ] = 1830;
  hole[ i++ ] = 2300;
  hole[ i++ ] = 2500;
  hole[ i++ ] = -1;

  i = 0;
  mount[ i++ ] = 35;
  mount[ i++ ] = 150;
  mount[ i++ ] = 210;
  mount[ i++ ] = 390;
  mount[ i++ ] = 520;
  mount[ i++ ] = 740;
  mount[ i++ ] = 810;
  mount[ i++ ] = 960;
  mount[ i++ ] = 1050;
  mount[ i++ ] = 1180;
  mount[ i++ ] = 1250;
  mount[ i++ ] = 1360;
  mount[ i++ ] = 1570;
  mount[ i++ ] = 1780;
  mount[ i++ ] = 1900;
  mount[ i++ ] = 2060;
  mount[ i++ ] = 2120;
  mount[ i++ ] = 2350;
  mount[ i++ ] = 2420;
  mount[ i++ ] = 2580;
  mount[ i++ ] = 2790;
  mount[ i++ ] = -1;

  int _k;
  for( _k=0; enemyX[_k] != -1; _k++ ){
    enemyVelocX[_k]= 1;
    enemySprite[_k]= 0;
    aniEnemy[_k]   = 0;
    enemyDead[_k]  = 0;
  }

  for( _k=0; coin[_k] != -1; _k++) coinSprite[_k] = 0;

  faseset( ' ' | COLOR_PAIR( CC ) );

  for( _k=0; _k < sizeFase; _k += 10 ){
    facecpy( HEIGHT - 3, _k, piso1, PISO );
    facecpy( HEIGHT - 2, _k, piso2, PISO );
    facecpy( HEIGHT - 1, _k, piso1, PISO );
  }


  for( _k=0; cloud[ _k ]  != -1; _k++ ){
    int _l;
    for( _l= 0; _l < 13; _l++ )
      facecpy( cloudY + _l, cloud[ _k ], &cloudDraw[_l][0], CLOUD );
  }


  for( _k=0; mount[_k] != -1; _k++ ){
    int _l;
    for( _l=0; _l < 7; _l++ )
      facecpy( mountY + _l, mount[ _k ], &mountDraw[ _l ][ 0 ], MOUNT );
  }

  for( _k=0; _k< 31; _k++ )
    facecpy( _k + 6, sizeFase - 120, &_castle[ _k ][ 0 ], CASTLE );

  flagY = 6;
}

void Splash(){
  screenGame = SPLASH;
  gamer.x = 0;
  gamer.y = 0;
  gamer.velY = 0;
  gamer.velX = 0;
  gamer.dead = 0;
  gamer.ani = 0;
  gamer.sprite = 0;
  gamer.side = D;

  //  screenset( ' ' | COLOR_PAIR( CC ) );

  int y =ats.initY + 12;
  int x =ats.widthMid - 20;

  int i;
  for( i = 0; i < 9; i++ ) screencpy( y++, x, &marioHead[ i ][ 0 ], MARIO );

  char lives[ 11 ];
  sprintf( lives, "X     0%02d", gamer.lifes );
  screencpy( y - 5, x + 20, lives, SCORE );

  Render();
  LoadFase();
  SLEEP( 1, 0 );
  screenGame = GAME;
  timeIniGame = SECONDS();
}

void Dead(){
  screenGame=DEAD;
  gamer.lifes--;
  if( gamer.lifes == 0 ){
  } else {
  }
  gamer.dead=1;
  gamer.y+=gravity;
}

int ColisaoBuraco(){
  int _k;
  for( _k=0; hole[ _k ] != -1; _k++ ){
    if( ( ( gamer.x + ( gamer.width / 2 ) ) > ( hole[_k] + 2 ) ) && ( ( gamer.x + ( gamer.width / 2 ) ) < ( hole[ _k ] + 2 + 25 ) ) ){
      gamer.x = hole[_k] + 2 + 12 - ( gamer.width / 2 );
      Dead();
      return 1;
    }
  }
  return 0;
}


int  ColisaoBloco(){
  if( block[ atblock ] == -2 ){
    return 0;
  }

  int _blocoX= block[atblock];

  if( gamer.x > ( _blocoX -1 + 15 ) ){ 
    return 0;
  }
  if( gamer.y > ( blockY -1 + 7 + ats.initY) ){
    return 0;
  }
  if( ( gamer.x -1 + gamer.width ) < _blocoX ){
    return 0;
  }
  if( ( gamer.y -1 + gamer.height ) < blockY + ats.initY ){
    return 0;
  }
  if( ( gamer.y - gamer.velY ) < ( blockY -1 + 7 + ats.initY) ){
    if( gamer.side == D ){
      gamer.x= _blocoX - 16;
    } else {
      gamer.x= _blocoX + 15;
    }
    gamer.velX=0;
    return 1;
  }
  Score( 50 );
  block[ atblock ]= -2;
  gamer.y= blockY + 7 + ats.initY;
  gamer.velY=- gamer.velY;
  int _k;
  for( _k= blockY; _k < ( blockY+7); _k++ ){
    facecpy( _k + ats.initY, _blocoX, _blocoClear, BLOCK );
  }

  return 1;
}

void ColisaoMastro(){
  if( ( gamer.x + gamer.width ) > 2880 ){
    screenGame=WIN;
    flagY=gamer.y;
    gamer.velY=0;
    int tmp= ( (21 - gamer.y ) * 330 ) + 50;
    sprintf( &_flagDraw[3][0], "%2s%6d%s", "X ", tmp, "   XXX" );
    Score( tmp );
    gamer.x=2883 - gamer.width;
  }
}

int ColisaoEnemy(){
  int _k;
  for( _k=0; enemyX[ _k ] != -1; _k++ ){
    if( ( enemyPathIni[_k] <  ( cameraX + ats.width ) ) && ( enemyPathFim[_k] > cameraX ) && ( enemySprite[_k] < 3 ) ){
      if( gamer.x > ( enemyX[_k] + 16 -1 ) ){
      } else if( gamer.y > ( enemyY + 10 -1 + ats.initY ) ){
      } else if( ( gamer.x + gamer.width -1 ) < enemyX[_k] ){
      } else if( ( gamer.y + gamer.height -1 ) < enemyY + ats.initY ){
      } else if( ( gamer.y + gamer.height -1 - gamer.velY ) < enemyY + ats.initY ){
        Score( 100 );
        gamer.y= enemyY - gamer.height + ats.initY;
        gamer.velY=-3;
        enemySprite[ _k ]= 3;
        enemyDead[_k]=1;
        return 1;
      } else {
        Dead(); 
        gamer.velY=-5;
        gamer.sprite=0;
        if(gamer.side == D ){
          gamer.x= enemyX[_k] - gamer.width;
        } else {
          gamer.x= enemyX[_k] + 16;
        }
        gamer.side=F;
        return 1;
      }
    }
 
  }
  return 0;
}

void GetCoin(){
  coins++;
  Score( 200 );
  if( coins > 99 ){
    coins-=100;
    gamer.lifes++;
  }
}

int ColisaoCoin(){
  if( coin[ atcoin ] == 0 ){
    return 0;
  }

  int _coinX= coin[ atcoin];
  if( gamer.x > ( _coinX + 10 ) ){
    return 0;
  }
  if( gamer.y > ( coinY + 5 + ats.initY ) ){
    return 0;
  }
  if( ( gamer.x + 15 ) < _coinX  ){
    return 0;
  }
  if( ( gamer.y + gamer.height ) < coinY + ats.initY ){
    return 0;
  }

  coin[ atcoin]=0;
  GetCoin();

  return 1;
}

void Gamer(){
  switch( screenGame ){
  case WINPOINT:
    if( timeGame > 0 ){
      Score( 10 );
      timeGame--;
    } else
      GameWin();
    break;
  case WIN:
    if( ( gamer.velY != 0 ) || ( ( gamer.y - 1 + gamer.height ) != ( piso - 1 ) ) ){
      gamer.sprite=2;
      gamer.velY += gravity;
      gamer.y += gamer.velY;
      flagY = gamer.y;

      if( ( gamer.y - 1 + gamer.height ) >= piso ){
        gamer.ani=0;
        gamer.sprite=0;
        gamer.y= piso - gamer.height;
        gamer.velY=0;
      }
    } else if( gamer.x < 2927 ){
      gamer.x++;
      gamer.ani++;
      if( gamer.ani == 2 ){
        gamer.sprite=0;
        gamer.ani=0;
      } else {
        gamer.sprite=1;
      }
    } else {
      gamer.x=3001;
      screenGame=WINPOINT;
    }
    break;
  case GAME:
  case DEAD:
    if( gamer.dead == 1 ){
      gamer.velY += gravity;
      gamer.y+=gamer.velY;
      if( gamer.y > 50 ){
        if( gamer.lifes == 0 ){
          //EEP( 1, 0 );
          GameOver();
        } else {
          //EEP( 2, 0 );
          Splash();
        }
      }
      return;
    }

    if( gamer.velX != 0 ){
      if( gamer.velX > 0 ){
        gamer.x+=4;
      } else {
        gamer.x-=4;
      }
      if( gamer.velX < 0 ){
        gamer.side=E;
        if( gamer.x < 0 ){
          gamer.x=0;
        }
        if( gamer.velY == 0 ){
          gamer.velX++;
        }
      } else {
        gamer.side=D;
        if( gamer.x > ( sizeFase - gamer.width ) ){
          gamer.x= sizeFase - gamer.width;
        }
        if( gamer.velY == 0 ){
          gamer.velX--;
        }
      }

      gamer.ani++;
      
      if( gamer.ani == 2 ){
        gamer.sprite=0;
        gamer.ani=0;
      } else {
        gamer.sprite=1;
      }
    } else {
      gamer.ani=0;
      gamer.sprite=0;
    }

    if( gamer.velY != 0 || ( ( gamer.y + gamer.height ) != ( piso + ats.initY ) ) ){
      gamer.sprite=2;
      gamer.velY+=gravity;
      gamer.y+=gamer.velY;
      if( ( gamer.y + gamer.height ) >= ats.initY + piso ){
        gamer.ani=0;
        gamer.sprite=0;

        if( ColisaoBuraco() == 0 ){
          gamer.y = ats.initY + piso - gamer.height;
          gamer.velY = 0;
        }
      }
    } else {
      ColisaoBuraco();
    }

    break;
  default: break;
  }
  
  if( screenGame == GAME ){
    ColisaoBloco();

    ColisaoCoin();
    ColisaoEnemy();
    ColisaoMastro();
  }

}



void ChangeSpriteEnemy( int x){
  int _k = x;
  enemyX[_k]+=enemyVelocX[_k];
  if( ( ( enemyX[_k] + 16 ) >  enemyPathFim[_k] ) || ( enemyX[_k] < enemyPathIni[_k] ) ){
    enemyVelocX[_k] = -enemyVelocX[_k];
  }
  aniEnemy[_k]=0;
}

void MoveEnemy(){
  if(  screenGame  == GAME ){
    int _k;
    for( _k=0; enemyX[ _k ] != -1; _k++ ){
      if( ( enemyPathIni[_k] < ( cameraX + ats.width ) ) && ( enemyPathFim[_k] > cameraX ) && ( enemySprite[_k] < 6 ) ){
        switch( enemySprite[_k] ){
        case 0:
          ChangeSpriteEnemy( _k);
          enemySprite[_k]=1;
          aniEnemy[_k]=1;
          break;
        case 1:
          ChangeSpriteEnemy( _k );
          enemySprite[ _k ] = 2;
          break;
        case 2:
          if( enemyDead[_k] == 1 ){
            enemySprite[_k]=3;
          }  else {
            ChangeSpriteEnemy( _k );
            enemySprite[_k]=0;
            aniEnemy[ _k ]=-1;
          }
          break;
        case 3:
         enemySprite[_k]=4; break;
        case 4:
          enemySprite[_k]=5; break;
        default:  break;
        }
      }

    }
  }
}



void InitGame(){
  score = 0;
  coins = 0;
  gamer.lifes = 3;
  Splash();
}


void ListenKey(){
  switch( tolower( getch() ) ){
  case ENTER:
    switch( screenGame ){
    case MENU: InitGame(); break;
    default:               break;
    }
    break;

  case ESC:
  case 'q':
    switch( screenGame ){
    case MENU: screenGame = QUIT; break;
    case GAME: LoadMenu();        break;
    default:                      break;
    }
    break;

  case ' '   :
  case KEY_UP:
    switch( screenGame ){
    case GAME:
      if( gamer.velY == 0 && ( ( gamer.y + gamer.height ) == piso + ats.initY ) ){
        gamer.velY = -6;
      }
      break;
    default:                      break;
    }
    break;
  case KEY_RIGHT:
    switch( screenGame ){
    case GAME: gamer.velX=4;  break;
    default:               break;
    }
    break;

  case KEY_LEFT:
    switch( screenGame ){
    case GAME: gamer.velX = -4;  break;
    default:                  break;
    }
    break;

  case 's':
    if( sound == 0 ){
      sound = 1;
      if( screenGame == GAME ){
      }
    } else {
      sound = 0;
    }
    break;

  case 'c':
    if( color ) color = 0;
    else        color = 1;
    break;
  }
}

void FPS(){
  static int ilps;
  static int ifps;

  ilps++;

  clock_t tempo2N = date();
  clock_t tempo = tempo2N - tempoN;

  if( tempo >= DECSEC * qs ){
    qs++;;
    ifps++;
    next = true;
    TerminalSize();
    switch( screenGame ){
    case GAME:
      timeGame = SECSGAME - ( SECONDS() - timeIniGame );
      if( timeGame == 30 ){
        if( fps == 10 ){
          //Play "timewarning"
        }
      } else if( timeGame == 27 ){
        if( fps == 10 ){
          //Play "background"
        }
      } else if( timeGame == 0 ){
        Dead();
        gamer.velY=-5;
        gamer.sprite=0;
        gamer.side=F;
      }
      break;
    default: break;
    }
  }

  if( tempo >= SECOND ){
    tempoN = tempo2N;
    lps = ilps;
    fps = ifps;
    ilps = 0;
    ifps = 0;
    qs = 1;
  }

}

int main(){
  start_curses();
  start_ck();

  timeIni = time( 0 );

  TerminalSize();

  screenset( ' ' | COLOR_PAIR( CC ) );
  faseset( ' ' | COLOR_PAIR( CC ) );

  LoadMenu();

  nodelay( stdscr, TRUE );
  GPL();

  tempoN = date();

  gamer.height = 16;
  gamer.width = 16;
  gamer.x = 0;
  gamer.y = 0;
  gamer.velY = 0;
  gamer.velX = 0;
  gamer.dead = 0;
  gamer.ani = 0;
  gamer.sprite = 0;
  gamer.side = D;

  while( screenGame != QUIT ){
    next = false;
    FPS();
    ListenKey();
    if( next ){
      MoveEnemy();
      Gamer();
      Render();
    }
  }

  end_curses();
  return 0;
}
