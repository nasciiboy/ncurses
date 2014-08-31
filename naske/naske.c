#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define T 50
#define FREE -10
#define LIMITW 55
#define LIMITH 21
#define DELAY 25000000
#define CASH 100
#define CAT( fil, cont )   strcpy( &cnaske[ fil ][ 0 ], #cont )

enum CK {  BB = 1, BR, BG, BY, BX, BM, BC, BW,
               RB, RR, RG, RY, RX, RM, RC, RW,
               GB, GR, GG, GY, GX, GM, GC, GW,
               YB, YR, YG, YY, YX, YM, YC, YW,
               XB, XR, XG, XY, XX, XM, XC, XW,
               MB, MR, MG, MY, MX, MM, MC, MW,
               CB, CR, CG, CY, CX, CM, CC, CW,
               WB, WR, WG, WY, WX, WM, WC, WW, CKMAX };

int main(){
  initscr();
  noecho();
  curs_set( FALSE );
  keypad( stdscr, TRUE );
  cbreak();
  nodelay( stdscr, TRUE );
  srand( time( 0 ) );
  start_color();
  if( !has_colors() ){
    fprintf( stderr, "No se logro inicializar el color." );
    endwin();
    exit( 1 );
  }

  int ci, cii, ck = 1;
  for( ci = 0; ci < 8; ci++ )
    for( cii = 0; cii < 8; cii++ )
      init_pair( ck++, ci, cii );

  bkgd( COLOR_PAIR( CB ) );

  char cnaske[ 25 ][ 83 ];
  CAT( 0,"... ...         .........                   ........         ..............  ...");
  CAT( 1,"... .  ............      .............             ..........     ......      ..");
  CAT( 2,"............. _|      _|              ...     _|             ......   ..........");
  CAT( 3,"............. _|_|    _|    _|_|_|    _|_|_|  _|  _|      _|_|             .....");
  CAT( 4,"............. _|  _|  _|  _|    _|  _|_|      _|_|      _|_|_|_|           .....");
  CAT( 5,"............. _|    _|_|  _|    _|      _|_|  _|  _|    _|            ..........");
  CAT( 6,"............. _|      _|    _|_|_|  _|_|_|    _|    _|    _|_|_|   .............");
  CAT( 7,"..             .....                                         ........         ..");
  CAT( 8,"..            ..       ..  ...                 ..............                 ..");
  CAT( 9,"..  ...........         .... .............                                ......");
  CAT(10,"......                                         |                 |     .........");
  CAT(11,"......   __ \    __|  _ \   __|   __|     __|  __|   _` |   __|  __|       .....");
  CAT(12,"......   |   |  |     __/ \__ \ \__ \   \__ \  |    (   |  |     |       ..   ..");
  CAT(13,"......   .__/  _|   \___| ____/ ____/   ____/ \__| \__,_| _|    \__|        ....");
  CAT(14,"......  _|                                                            ....... ..");
  CAT(15,"....          . ............. .        ...............                      . ..");
  CAT(16,"....               ..          ........         ....  .....             ....  ..");
  CAT(17,".....          .....  .    _   _   _   _   _   _   ..        ... ...........  ..");
  CAT(18,".....   ........   ....   / \ / \ / \ / \ / \ / \   ........        ...       ..");
  CAT(19,".. ..            ...     ( b | u | t | t | o | m )         .    ................");
  CAT(20,".. . .       ....         \_/ \_/ \_/ \_/ \_/ \_/       .. ........          ...");
  CAT(21,".... .   .....  .....                            ........                  .....");
  CAT(22,".... .    .......              ................ ..    ..        ...........   ..");
  CAT(23,".... ..     ...............  ..........................   ......................");
  CAT(24,"....... ...... ................    .  .........     ............................");

    int
    key,
    x = 10, y = 10,
    end = 1, lose = 0, dead = 0,
    crash = 0, crashx, crashy,
    vel = 10,
    direccion = 0,
    naskex[ T ], naskey[ T ],
    end_naske, end_naskex, end_naskey,
    size = 0, score = 0,
    cont = 0, i,
    food = 0, foodx = 0, foody = 0,
    prize = 0, prizex = 0, prizey = 0, prizeid = -1, prizemult = 0, rm_mesprize = 0;
    timespec req;
    req.tv_sec = 0;


  if( LINES < 25 || COLS < 80 ){
    while( LINES < 25 || COLS < 80 ){
      erase();
      mvprintw( LINES/2, COLS/3 - 5, "LINES %3d >= 25 -- COLS %3d >= 80", LINES, COLS );
      getch();
        refresh();
    }
    clear();
  }

  attron( COLOR_PAIR( WG ) );
  for( ci = 0; ci < 25; ci++ ){
    for( cii = 1; cii < 81; cii++ )
      mvprintw( ci, cii - 1, "%c", cnaske[ ci ][ cii ] );
  }
  attroff( COLOR_PAIR( WG ) );
  refresh();
  nodelay( stdscr, FALSE );
  getch();
  nodelay( stdscr, TRUE );
  clear();
  
  for( i = 0; i < T; i++ )
    naskex[ i ] = naskey[ i ] = FREE;

  // INFO
  attron( A_BOLD );
  mvprintw(  0, 20, "N  A  S  K  E" );
  attroff( A_BOLD );
  attron( COLOR_PAIR( WB ) );
  mvprintw( 24,  3, "food:" );
  attroff( COLOR_PAIR( WB ) );
  attron( COLOR_PAIR( BW ) );
  mvprintw( 24, 10, "0" );
  attroff( COLOR_PAIR( BW ) );
  attron( COLOR_PAIR( WB ) );
  mvprintw( 24, 20, "cash:" );
  attroff( COLOR_PAIR( WB ) );
  attron( COLOR_PAIR( WG ) | A_BOLD );
  mvprintw( 24, 27, "$" );
  attroff( COLOR_PAIR( WG ) | A_BOLD );
  attron( COLOR_PAIR( WB ) );
  mvprintw( 24, 37, "mult:" );
  attroff( COLOR_PAIR( WB ) );
  attron( COLOR_PAIR( WR ) );
  mvprintw( 24, 44, "*" );
  attroff( COLOR_PAIR( WR ) );
  attron( COLOR_PAIR( WB ) );
  mvprintw( 24, 54, "rand:" );
  attroff( COLOR_PAIR( WB ) );
  attron( COLOR_PAIR( RY ) | A_BOLD );
  mvprintw( 24, 61, "?" );
  attroff( COLOR_PAIR( RY ) | A_BOLD );
  attron( COLOR_PAIR( WB ) );
  mvprintw( 24, 66, "DELBACK exit" );
  attroff( COLOR_PAIR( WB ) );

  // LIMITS
  attron( COLOR_PAIR( XB ) );
  mvhline( 1, 0, ACS_HLINE, LIMITW );
  mvhline( LIMITH + 2, 0, ACS_HLINE, LIMITW );
  mvvline( 2, 0, ACS_VLINE, LIMITH );
  mvvline( 1, LIMITW + 1, ACS_VLINE, LIMITH + 2 );
  attroff( COLOR_PAIR( XB ) );

  // GAME-LOOP
  while( end ){
    if( LINES < 25 || COLS < 80 ){
      while( LINES < 25 || COLS < 80 ){
        erase();
        mvprintw( LINES/2, COLS/3 - 3, "LINES %d >= 25 -- COLS %d >= 80", LINES, COLS );
        getch();
        refresh();
      }
      clear();
      // INFO
      attron( A_BOLD );
      mvprintw(  0, 20, "N  A  S  K  E" );
      attroff( A_BOLD );
      attron( COLOR_PAIR( WB ) );
      mvprintw( 24,  3, "food:" );
      attroff( COLOR_PAIR( WB ) );
      attron( COLOR_PAIR( BW ) );
      mvprintw( 24, 10, "0" );
      attroff( COLOR_PAIR( BW ) );
      attron( COLOR_PAIR( WB ) );
      mvprintw( 24, 20, "cash:" );
      attroff( COLOR_PAIR( WB ) );
      attron( COLOR_PAIR( WG ) | A_BOLD );
      mvprintw( 24, 27, "$" );
      attroff( COLOR_PAIR( WG ) | A_BOLD );
      attron( COLOR_PAIR( WB ) );
      mvprintw( 24, 37, "mult:" );
      attroff( COLOR_PAIR( WB ) );
      attron( COLOR_PAIR( WR ) );
      mvprintw( 24, 44, "*" );
      attroff( COLOR_PAIR( WR ) );
      attron( COLOR_PAIR( WB ) );
      mvprintw( 24, 54, "rand:" );
      attroff( COLOR_PAIR( WB ) );
      attron( COLOR_PAIR( RY ) | A_BOLD );
      mvprintw( 24, 61, "?" );
      attroff( COLOR_PAIR( RY ) | A_BOLD );
      attron( COLOR_PAIR( WB ) );
      mvprintw( 24, 66, "DELBACK exit" );
      attroff( COLOR_PAIR( WB ) );
      // LIMITS
      attron( COLOR_PAIR( XB ) );
      mvhline( 1, 0, ACS_HLINE, LIMITW );
      mvhline( LIMITH + 2, 0, ACS_HLINE, LIMITW );
      mvvline( 2, 0, ACS_VLINE, LIMITH );
      mvvline( 1, LIMITW + 1, ACS_VLINE, LIMITH + 2 );
      attroff( COLOR_PAIR( XB ) );

      if( prize ){
        switch( prizeid ){
        case 0:
          attron( COLOR_PAIR( WR ) );
          mvprintw( prizey, prizex, "*" );
          attroff( COLOR_PAIR( WR ) );
          break;
        case 1:
          attron( COLOR_PAIR( WG ) | A_BOLD );
          mvprintw( prizey, prizex, "$" );
          attroff( COLOR_PAIR( WG ) | A_BOLD );
          break;
        case 2:
          attron( COLOR_PAIR( RY ) | A_BOLD );
          mvprintw( prizey, prizex, "?" );
          attroff( COLOR_PAIR( RY ) | A_BOLD );
          break;
        }
      }
      if( direccion != 0 ){
        attron( COLOR_PAIR( BW ) );
        mvprintw( foody, foodx, "0" );
        attroff( COLOR_PAIR( BW ) );
      }
    }

    naskex[ cont ] = x; naskey[ cont ] = y;
    end_naske = cont - size;
    if( end_naske < 0 ) end_naske += T;
    end_naskex = naskex[ end_naske ];
    end_naskey = naskey[ end_naske ];
    if( ++cont == T ) cont = 0;

    // SCORE
    mvprintw(   5, 63, "score" );
    attron( COLOR_PAIR( BW ) );
    mvprintw(   6, 63, "%10d", score );
    attroff( COLOR_PAIR( BW ) );
    mvprintw(  10, 63, "size: " );
    attron( COLOR_PAIR( YB ) );
    mvprintw(  10, 69, "%3d", size + 1 );
    attroff( COLOR_PAIR( YB ) );
    mvprintw(  14, 64, "vel: " );
    attron( COLOR_PAIR( YB ) );
    mvprintw(  14, 69, "%3d", 11 - vel );
    attroff( COLOR_PAIR( YB ) );
    attron( COLOR_PAIR( YB ) );
    mvprintw(  20, 60, "y: %2d", y );
    attroff( COLOR_PAIR( YB ) );
    attron( COLOR_PAIR( GB ) );
    mvprintw(  20, 70, "x: %2d", x );
    attroff( COLOR_PAIR( GB ) );



    // control
    key = getch();
    switch( key ){
    case KEY_DOWN:  direccion = 1; break;
    case KEY_UP:    direccion = 2; break;
    case KEY_RIGHT: direccion = 3; break;
    case KEY_LEFT:  direccion = 4; break;
    case KEY_BACKSPACE:   end = 0; break;
    case 'q':             end = 0; break;
    default:                       break;
    }

    // movimiento
    switch( direccion ){
    case 1: y++;            req.tv_nsec = vel * DELAY; nanosleep( &req, 0 ); break;
    case 2: y--;            req.tv_nsec = vel * DELAY; nanosleep( &req, 0 ); break;
    case 3: x++; req.tv_nsec = vel * DELAY - 10000000; nanosleep( &req, 0 ); break;
    case 4: x--; req.tv_nsec = vel * DELAY - 10000000; nanosleep( &req, 0 ); break;
    default:                                    break;
    }

    // limites "dimencionales"
    if     ( x == LIMITW + 1 ) x = 1;
    else if( x == 0 ) x = LIMITW;
    else if( y == LIMITH + 2 ) y = 2;
    else if( y == 1 ) y = LIMITH + 1;

    // borrar
    naskex[ end_naske ] = naskey[ end_naske ] = FREE;

    // food
    while( food == 0 && direccion != 0 ){
      do{ foodx = rand() %  LIMITW  + 1; } while( foodx == prizex );
      do{ foody = rand() %  LIMITH  + 2; } while( foody == prizey );
      int sent = 0;
      for( i = 0; i < T; i++ )
        if( naskex[ i ] == foodx && naskey[ i ] == foody ) sent = 1;
      if( !sent ){
        attron( COLOR_PAIR( BW ) );
        mvprintw( foody, foodx, "0" );
        attroff( COLOR_PAIR( BW ) );
        food = 1;
      }
    }

    // prize
    if( !prize ){
      int prize_rand = rand() % 2000;
      if( prize_rand < 50 ){
        while( prize == 0 && direccion != 0 ){
          do{ prizex = rand() % LIMITW + 1; } while( prizex == foodx );
          do{ prizey = rand() % LIMITH + 2; } while( prizey == foody );
          int sent = 0;
          for( i = 0; i < T; i++ )
            if( naskex[ i ] == foodx && naskey[ i ] == foody ) sent = 1;
          if( !sent ){
            switch( prize_rand % 2 ) {
            case 0:
              attron( COLOR_PAIR( WG ) | A_BOLD );
              mvprintw( prizey, prizex, "$" );
              attroff( COLOR_PAIR( WG ) | A_BOLD );
              prizeid = 1;
              prize = 1;
              break;
            case 1:
              attron( COLOR_PAIR( RY ) | A_BOLD );
              mvprintw( prizey, prizex, "?" );
              attroff( COLOR_PAIR( RY ) | A_BOLD );
              prizeid = 2;
              prize = 1;
              break;
            default: break;
            }
          }
        }
      } else if( prize_rand >= 50 && prize_rand < 60 ){
        while( prize == 0 && direccion != 0 ){
          do{ prizex = rand() % LIMITW + 1; } while( prizex == foodx );
          do{ prizey = rand() % LIMITH + 2; } while( prizey == foody );

          int sent = 0;
          for( i = 0; i < T; i++ )
            if( naskex[ i ] == foodx && naskey[ i ] == foody ) sent = 1;
          if( !sent ){
            attron( COLOR_PAIR( WR ) );
            mvprintw( prizey, prizex, "*" );
            attroff( COLOR_PAIR( WR ) );
            prizeid = 0;
            prize = 1;
          }
        }
      }
    }

    // crash
    if( !crash )
      for( i = 0; i < T; i++ )
        if( x == naskex[ i ] && y == naskey[ i ] ){
          crash = 1; crashx = x; crashy = y; dead = size; vel = 10;
        }

    // mover
    if( crash != 1 ){
      attron( COLOR_PAIR( GB ) );
      mvprintw( y, x, "#" );
      attroff( COLOR_PAIR( GB ) );
    } else if( crash ){
      if( dead-- == -1 ) lose = 1;
      y = crashy; x = crashx;
    }

    // borrar
    if( direccion != 0 ) mvprintw( end_naskey, end_naskex, " " );

    // eat
    if( !crash && x == foodx && y == foody ){
      if( prizemult ){
        size = ( size + prizemult > T - 1 ) ? T - 1 : size + prizemult;
        mvprintw( 3, 65, "size +%-6d", prizemult );
        prizemult = 0;
        rm_mesprize = 10;
      } else size++;
      food = 0;
    }
    if( x == prizex && y == prizey ){
      if( prizemult ){
        switch( prizeid ){
        case 0:
          prizemult *= prizemult;
          mvprintw( 3, 65, "next x%-6d", prizemult );
          break;
        case 1:
          score +=  CASH * prizemult;
          mvprintw( 3, 65, "+%-11d", CASH * prizemult );
          prizemult = 0;
          break;
        case 2:
          switch( cont % 4 ){
          case 0:
            vel = ( vel - prizemult > 0 ) ? vel - prizemult : 1;
            mvprintw( 3, 65, "vel+%-8", prizemult );
            prizemult = 0; break;
          case 1:
            vel = ( vel + prizemult <= 10 ) ? vel + prizemult : 10;
            mvprintw( 3, 65, "vel-%-8", prizemult );
            prizemult = 0; break;
          case 2:
            score += CASH * prizemult;
            mvprintw( 3, 65, "+%-11d", CASH * prizemult );
            prizemult = 0; break;
          case 3:
            prizemult *= prizemult;
            mvprintw( 3, 65, "next x%-6d", prizemult );
            break;
          }
          break;
        }
      } else {
        switch( prizeid ){
        case 0:
          prizemult = rand() % 3 + 2;
          mvprintw( 3, 65, "next x%-6d", prizemult );
          break;
        case 1:
          score +=  CASH;
          mvprintw( 3, 65, "+%-11d", CASH );
          break;
        case 2:
          switch( cont % 4 ){
          case 0:
            vel = ( vel - 1 > 0 )   ? vel - 1 :  1;
            mvprintw( 3, 65, "vel+        " );
            break;
          case 1:
            vel = ( vel + 1 <= 10 ) ? vel + 1 : 10;
            mvprintw( 3, 65, "vel-        " );
            break;
          case 2:
            score += CASH;
            mvprintw( 3, 65, "+%-11d", CASH );
            break;
          case 3:
            prizemult = rand() % 3 + 2;
            mvprintw( 3, 65, "next x%-5d", prizemult );
            break;
          }
          break;
        }
      }
      prize = 0;
      if( !prizemult ) rm_mesprize = 10;
    }

    if( !--rm_mesprize ) mvprintw( 3, 65, "            " );


    // GANAR
    if( size >= T - 1 ){
      clear();
      attron( COLOR_PAIR( RB ) | A_BOLD );
      mvprintw( 15, 30, "YOU WIN" );
      attroff( COLOR_PAIR( RB ) | A_BOLD );
      mvprintw( 16, 5, " SIZE %10d", size + 1 );
      mvprintw( 17, 5, "SCORE  %10d", score );
      nodelay( stdscr, FALSE );
      getch();
      end = 0;
    }
    // PERDER
    if( lose ){
      clear();
      attron( COLOR_PAIR( RB ) | A_BOLD );
      mvprintw( 15, 10, "YOU LOSE" );
      attroff( COLOR_PAIR( RB ) | A_BOLD );
      mvprintw( 16, 5, " SIZE %10d", size + 1 );
      mvprintw( 17, 5, "SCORE %10d", score );
      nodelay( stdscr, FALSE );
      getch();
      end = 0;
    }

    refresh();
  }

  endwin();
  return 0;
}
