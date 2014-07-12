////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//    Copyright (C) 2014 nasciiBoy                                                //
//                                                                                //
//    This program is free software: you can redistribute it and/or modify        //
//    it under the terms of the GNU General Public License as published by        //
//    the Free Software Foundation, either version 3 of the License, or           //
//    (at your option) any later version.                                         //
//                                                                                //
//    This program is distributed in the hope that it will be useful,             //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of              //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               //
//    GNU General Public License for more details.                                //
//                                                                                //
//    You should have received a copy of the GNU General Public License           //
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.       //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// dimencion total
#define HEIGHT 25
#define WIDTH  80

// dimencion de laverinto
#define MAP_HSIZE  25
#define MAP_WSIZE  79
// posicion del laverinto en pantalla
#define MAP_X       0
#define MAP_Y       0

// valor para el refresco de pantalla
#define HZ        25000000

// posibles valores que tomara el arreglo que representa el laverinto
#define VISIT    1  // posicion visitada
#define EMPTY   ' ' // camino
#define DIRT    '#' // muro blando
#define WALL    '|' // muro
#define PLAYER  'Z' // jugador
#define MONSTER 'M' // mostruo
#define BLOOD   '+' // sangre de jugador

// para manejar los movimientos posibles de manera comoda
enum      MV      {  UP, LEFT, DOWN, RIGHT };
const int dx[4] = {   0,   -1,    0,     1 };
const int dy[4] = {  -1,    0,    1,     0 };

// almaecna una posicion
typedef struct {
  int y, x;
} gps;

// oara pausar segundos y nanosegundos
void SLEEP( int sec, int nsec ){
  timespec req;
  req.tv_sec  =  sec;
  req.tv_nsec = nsec;
  nanosleep( &req, 0 );
}

// inicia el majejo de pantalla
void start_curses(){
  initscr();
  noecho();
  curs_set( FALSE );
  cbreak();
  keypad( stdscr, TRUE );
}

// termina manejo de pantalla
void end_curses(){
  endwin();
}

// para mejar los colores disponibles de forma comoda
// el primer valor representa el color del caracter
// el segundo el color de fondo
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

// inicializa el uso de color
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

// determina si las dimenciones establecidas para la aplicacion son cumplidas
void terminalsize(){
  if( LINES < HEIGHT || COLS < WIDTH ){
    end_curses();
    fprintf( stderr, "TERMINAL MÍNIMUM %d LINES x %d COLUMNS!\nNOW %d LINES x %d COLUMNS.\n",
             HEIGHT, WIDTH, LINES, COLS );
    exit( 1 );
  }
}

// inicializa a lo gameboy
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

  int nu_y  = ( LINES - NU_H ) / 2;
  int nu_x  = ( COLS  - 18   ) / 2;

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
}

// pone en pantalla un mensage
int msgbox( const char* msg ){
  if( msg ){
    int width = strlen( msg );
    WINDOW* msgwin = newwin( 3, width + 2, HEIGHT / 2, ( WIDTH - width ) / 2 );
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
    WINDOW* msgwin = newwin( 3, width + 2, HEIGHT / 2, ( WIDTH - width ) / 2 );
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

// carga un fichero de texto y lo dibuja en pantalla
int draw_file( WINDOW* win, const char * file, int y, int x ){
  FILE * fp = fopen( file, "r" );
  if( fp == 0 ) return 0;

  if( win == 0 ) win = stdscr;
  int width  = getmaxx( win );
  int height = getmaxy( win );

  char ch;
  int ix = x, iy = y;
  while( ( ch = fgetc( fp ) ) != EOF ){
    if( ch == '\n' ){
      iy++; ix = x;
    } else {
      if( ix >= 0 && ix < width && iy >= 0 && iy < height ){
        if( ch == ' ' ){
        } else mvwaddch( win, iy, ix, ch );
      }
      ix++;
    }
  }
  fclose( fp );
  return 1;
}

///////////////////////////////////////////////////////////////////////////// MENU
int menu(){
  int ii;
  attrset( COLOR_PAIR( WW ) | A_BOLD );
  for( ii = 0; ii < 45; ii++ ){
    clear();
    draw_file( stdscr, "npath", 3, ii );
    refresh();
    SLEEP( 0, 9000000 );
  }

  attrset( COLOR_PAIR( GB ) | A_BOLD );
  draw_file( stdscr, "start", 15, 15 );
  refresh();
  SLEEP( 0, 50000000 );


  attrset( COLOR_PAIR( MB ) );
  mvprintw(  HEIGHT - 1,  0, "Distributed under the terms of the GPL license");

  nodelay( stdscr, FALSE );
  switch( tolower( getch()  ) ){
  case 'q'      : return 0;
  case 27       : return 0;
  }

  return 1;
}

/////////////////////////////// para el manejo del arreglo que representa el laverinto

// carga el contenido de un fichero a un arreglo
int load_map( const char* file, char map[MAP_HSIZE][MAP_WSIZE] ){
  FILE* fp = fopen( file, "r" );
  if( fp == 0 ){ 
    char msg[ 1048 ];
    sprintf( msg, "no load file \"%s\"", file );
    msgbox( msg );
    end_curses();
    exit( 1 );
  }

  memset( map, EMPTY, sizeof(char) * MAP_HSIZE * MAP_WSIZE );

  char ch;
  int i = 0, ii = 0;
  while( ( ch = fgetc( fp ) ) != EOF ){
    if( ch == '\n' ){ 
      if( ++i >= MAP_HSIZE ){ fclose( fp ); return 1; }
      ii = 0;
    } else {
      if( ii < MAP_WSIZE ) map[i][ii] = ch;
      ii++;
    }
  }

  fclose( fp );
  return 1;
}

// guarda el contenido de un arreglo a un fichero
int save_map( const char* file, char map[MAP_HSIZE][MAP_WSIZE] ){
  FILE* fp = fopen( file, "w" );
  if( fp == 0 ) return 0;

  int i, ii;
  for(i = 0; i < MAP_HSIZE; i++){
    for(ii = 0; ii < MAP_WSIZE; ii++)
      fputc( map[i][ii], fp );
    fputc( '\n', fp );
  }

  fclose( fp );
  return 1;
}

// dibuja un arreglo en pantalla
void draw_map( char map[MAP_HSIZE][MAP_WSIZE], int y, int x ){
  int i, ii;
  for(i = 0; i < MAP_HSIZE; i++)
    for(ii = 0; ii < MAP_WSIZE; ii++)
      switch( map[i][ii] ){
      case RIGHT  : mvaddch( y + i, x + ii, '>' | COLOR_PAIR( GB )); break;
      case UP     : mvaddch( y + i, x + ii, '^' | COLOR_PAIR( YB )); break;
      case LEFT   : mvaddch( y + i, x + ii, '<' | COLOR_PAIR( CB )); break;
      case DOWN   : mvaddch( y + i, x + ii, 'v' | COLOR_PAIR( MB )); break;
      case 4      : mvaddch( y + i, x + ii, ' ' | COLOR_PAIR( XX )); break;

      case EMPTY  : mvaddch( y + i, x + ii, EMPTY   | COLOR_PAIR( WB )            ); break;
      case DIRT   : mvaddch( y + i, x + ii, DIRT    | COLOR_PAIR( RB ) | A_NORMAL ); break;
      case WALL   : mvaddch( y + i, x + ii, WALL    | COLOR_PAIR( CB ) | A_NORMAL ); break;
      case PLAYER : mvaddch( y + i, x + ii, PLAYER  | COLOR_PAIR( WB ) | A_BOLD   ); break;
      case MONSTER: mvaddch( y + i, x + ii, MONSTER | COLOR_PAIR( MB ) | A_BOLD   ); break;
      case BLOOD  : mvaddch( y + i, x + ii, BLOOD   | COLOR_PAIR( YB ) | A_BOLD   ); break;
      default     : mvaddch( y + i, x + ii, '?'                                   ); break;
      }
}

// cambia un elemento del arreglo por otro en especifico o establece uno nuevo
int change_chmap( char map[MAP_HSIZE][MAP_WSIZE], int y, int x, int nw, int prew ){
  if( x < 0 || y < 0 || x >= MAP_WSIZE || y >=  MAP_HSIZE ) return 0;

  char ch = map[y][x];
  if( prew == 0 ){
    map[y][x] = nw;
    return ch;
  } else if( map[y][x] == prew ){
    map[y][x] = nw;
    return 1;
  } else return ch;
}

////////////////////////////////////////////////// EDITOR
// dibuja un caracter en pantalla
void draw_chwin( WINDOW* win, int y, int x, int ch ){
  if( win == 0 ) win = stdscr;
  switch( ch ){
  case EMPTY  :mvwaddch( win, y, x, EMPTY  |COLOR_PAIR( WB )           );break;
  case DIRT   :mvwaddch( win, y, x, DIRT   |COLOR_PAIR( RB )| A_NORMAL );break;
  case WALL   :mvwaddch( win, y, x, WALL   |COLOR_PAIR( CB )| A_NORMAL );break;
  case PLAYER :mvwaddch( win, y, x, PLAYER |COLOR_PAIR( WB )| A_BOLD   );break;
  case MONSTER:mvwaddch( win, y, x, MONSTER|COLOR_PAIR( MB )| A_BOLD   );break;
  case BLOOD  :mvwaddch( win, y, x, BLOOD  |COLOR_PAIR( YB )| A_BOLD   );break;
  default     :mvwaddch( win, y, x, ch                                 );break;
  }
}

// salva en arreglo a fichero maze e informa el resultado
int save( char map[MAP_HSIZE][MAP_WSIZE] ){
  if( save_map( "maze", map ) ){
    msgbox( "Saved successfully!" );
    return 1;
  } else {
    msgbox( "ERR: Unable to open file for writing!" );
    return 0;
  }
}

//-------> relacionado con la generacion del laverinto
// structura para manejar una pila
struct stack {
  int x, y;           /* coordenadas del ultimo nodo visitado */
  struct stack *next; /* puntero al nodo anterior      */
};

// ingresa un nuevo nodo
int push( struct stack **top, const int x, const int y ) {
  struct stack *node = (stack*)malloc( sizeof( stack ) );
  if( node ){
    node->x = x;
    node->y = y;
    node->next = *top;
    *top = node;
    return 1;
  } return 0;
}

// libera nodo actual y regresa al anterior
void pop( struct stack **top ){
  if( *top ){
    struct stack * tmp = *top;
    *top = tmp->next;

    free( tmp );
  }
}

// libera la pila
void free_stack( struct stack **top ){
  while( *top ) pop( top );
}

/* busca de forma aleatoria un vecino disponible,
   si no hay devuelve cero */
int rand_neighbour( char maze[][ MAP_WSIZE ], int * const x, int * const y ){
  char cdir[ 4 ] = { 1, 1, 1, 1 }; // representan la direciones posibles

  while( cdir[ UP ] || cdir[ LEFT ] || cdir[ DOWN ] || cdir[ RIGHT ] ){
    int mv = rand() % 4;

    // comprueba la direccion
    if( cdir[ mv ] ){
      // establece le valor interno para la nueva direccion
      int iy = *y + dy[ mv ] * 2;
      int ix = *x + dx[ mv ] * 2;

      // verifica los limites
      if( ix < 0 || iy < 0 || ix >= MAP_WSIZE || iy >= MAP_HSIZE ) cdir[ mv ]  = 0;
      else {
        // si el vesino esta vacio( no visitado )
        if( maze[ iy ][ ix ] == EMPTY ){
          // establece el muro intermedio como visitado
          maze[ *y + dy[ mv ] ][ *x + dx[ mv ] ] = VISIT;
          // establecemos las nuevas coordenadas
          *y += dy[ mv ] * 2;
          *x += dx[ mv ] * 2;
          return 1; // regresa 1
        } else {  cdir[ mv ] = 0; // si sale de los limites marca direccion como comprobada
        }
      }  // else
    }    // while
  }      // if cdir[ mv ]

  return 0; // regresa 0
}

// genera el laverinto
int generate( char maze[][ MAP_WSIZE ] ){
  int i, ii;

  /* se establece una configuracion apropiada para el laverinto segun su dimencion
     de forma que las posiciones se intercalen muro-espacio-muro */
  for( i = 0; i < MAP_HSIZE; i++ )
    for( ii = 0; ii < MAP_WSIZE; ii++ ){
      if( MAP_HSIZE % 2 == 0 ){
        if( MAP_WSIZE % 2 == 0 ){
          if( ( i + 1 ) % 2 == 0 || ( ii + 1 ) % 2 == 0 ) maze[i][ii] = WALL;
          else                                            maze[i][ii] = EMPTY;
        } else {
          if( ( i + 1 ) % 2 == 0 ||   ii       % 2 == 0 ) maze[i][ii] = WALL;
          else                                            maze[i][ii] = EMPTY;
        }
      } else { 
        if( MAP_WSIZE % 2 == 0 ){
          if( i         % 2 == 0 || ( ii + 1 ) % 2 == 0 ) maze[i][ii] = WALL;
          else                                            maze[i][ii] = EMPTY;
        } else {
          if( i         % 2 == 0 || ii         % 2 == 0 ) maze[i][ii] = WALL;
          else                                            maze[i][ii] = EMPTY;
        }
      }
    }

  struct stack *st = NULL;
  int x, y, tmpx, tmpy;

  srand( time( 0 ) );

  // posicion inicial de partida
  x = MAP_WSIZE - 2;
  y = MAP_HSIZE - 2;

  int run = 1;
  while( run ){
    // marcamos como visitado
    maze[y][x] = VISIT;
    /* si existe un vecino x e y son modificados, con estas variables
       tenemos la pocion de partida( modo padre ) */
    tmpx = x;
    tmpy = y;

    // si hay un vecino disponible
    if( rand_neighbour( maze, &x, &y ) ){
      // guardamos el nodo si hay memoria disponible
      if( !push(&st, tmpx, tmpy)){
        // si no hay memoria disponible liberamos la pila
        free_stack(&st);
        return 0;
      }
    } else {
      // si no tenemos un vecino disponible regresamos al nodo anterior
      if( st ){
        x = st->x;
        y = st->y;

        pop( &st );

        // si es el final de la pila terminamos
        if( st->next == 0 ) run = 0;
      } else run = 0;;
    }
  } // while run

  // establecemos al jugador y un mostruo
  maze[1][0] = PLAYER;
  maze[ MAP_HSIZE - 2][ MAP_WSIZE - 1] = MONSTER;

  // establecemos las posiciones visitadas como camino
  for( i = 0; i < MAP_HSIZE; i++ )
    for( ii = 0; ii < MAP_WSIZE; ii++ ){
      if( maze[ i  ][ ii ] == VISIT ) maze[ i  ][ ii ] = EMPTY;
    }

  return 1; // regresa 1
}

////////////////////////////////////////////////// PATHFIND
/* mediante el mapa - laberinto - y la posicion del jugador generaremos todos
   los caminos posibles de este ultimo y los almacenamos en un mapa con "direciones - flecha"
   a forma de tecnica miga de pan */
int path_find( const char map[MAP_HSIZE][MAP_WSIZE], char arrow_map[MAP_HSIZE][MAP_WSIZE],
               gps init ){
  // limpiamos el mapa de direcciones
  memset( arrow_map, 4, sizeof(char) * MAP_HSIZE * MAP_WSIZE );


  int change = 0;   // almacena si hay un cambio
  int y, x;         // almacena la procima direccion
  int i, ii, dir;   // para contorlas los recorridos ataves de los arreglos

  // primer ciclo para "arrancar la maqinaria de flechas - migas -"
  for( dir = 0; dir < 4; dir++ ){
    // establecemos la procima direcion
    y = init.y + dy[dir];
    x = init.x + dx[dir];

    // verificamos los limites
    if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
      // out of limits
    } else {
      if( map[y][x] )
        // dejamos una miga con la direccion
        switch( dir ){
        case RIGHT: arrow_map[y][x] = LEFT ; change = 1; break;
        case UP   : arrow_map[y][x] = DOWN ; change = 1; break;
        case LEFT : arrow_map[y][x] = RIGHT; change = 1; break;
        case DOWN : arrow_map[y][x] = UP   ; change = 1; break;
        }
    }
  }

  // si la maquina encendio vamos a por todo el mapa
  while( change ){
    change = 0;

    /* con este metodo recorremos de ariiba a abajo comprobando las
       cuatro posibles direcciones para cada posicion */
    /*
    // RIGHT -> DOWN -> 4D
    for( i = 0; i < MAP_HSIZE; i++ )
      for( ii = 0; ii < MAP_WSIZE; ii++ ){
        if( arrow_map[i][ii] < 4 ){
          for( dir = 0; dir < 4; dir++ ){
            y = i  + dy[dir];
            x = ii + dx[dir];
            if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
              // out of limits
            } else {
              if( map[y][x] && arrow_map[y][x] == 4 )
                switch( dir ){
                case RIGHT: arrow_map[y][x] = LEFT ; change = 1; break;
                case UP   : arrow_map[y][x] = DOWN ; change = 1; break;
                case LEFT : arrow_map[y][x] = RIGHT; change = 1; break;
                case DOWN : arrow_map[y][x] = UP   ; change = 1; break;
                } // switch
            }     // else
          }       // for( dir )
        }         // if( arrow_map < 4 )
      }           // for( ii )
    
    */


    /* la suiguiente forma de revisar el laverinto es la que mas ciclos requiere
       pera genera un resultado equivalente a el algoritmo A* */
    /* con este primer ciclo recorremos de arriba a abajo - izquierda a derecha -
       comprobando solo la casilla de arriba y de la izquierda( en direccion
       opuesta a la del ciclo ) */
    // RIGHT -> DOWN ->  -- UP -- LEFT --
    for( i = 0; i < MAP_HSIZE; i++ )
      for( ii = 0; ii < MAP_WSIZE; ii++ ){
        // si la casiila se ha visitado - es una miga -
        if( arrow_map[i][ii] < 4 ){
          // establecemos la nueva posiion a verificar
          y = i  + dy[UP];
          x = ii + dx[UP];
          if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
            // out of limits
          } else {
            // si esta dentro de los limites y no a sido establecida
            if( map[y][x] && arrow_map[y][x] == 4 ){
              arrow_map[y][x] = DOWN ; change = 1;
            }
          }     // else

          y = i  + dy[LEFT];
          x = ii + dx[LEFT];
          if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
            // out of limits
          } else {
            if( map[y][x] && arrow_map[y][x] == 4 ){
              arrow_map[y][x] = RIGHT; change = 1;
            }
          }       // else
        }         // if( arrow_map < 4 )
      }           // for( ii )

    /* con este segundo ciclo recorremos de abajo a arriba - derecha a izquierda -
       comprobando solo la casilla de abajo y de la derecha( en direccion
       opuesta a la del ciclo ) */
    // LEFT -> UP ->  -- DOWN -- RIGHT --
    for( i = MAP_HSIZE - 1; i >= 0; i-- )
      for( ii = MAP_WSIZE - 1; ii >= 0; ii-- ){
        if( arrow_map[i][ii] < 4 ){
          y = i  + dy[DOWN];
          x = ii + dx[DOWN];
          if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
            // out of limits
          } else {
            if( map[y][x] && arrow_map[y][x] == 4 ){
              arrow_map[y][x] = UP; change = 1;
            }
          }     // else

          y = i  + dy[RIGHT];
          x = ii + dx[RIGHT];
          if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
            // out of limits
          } else {
            if( map[y][x] && arrow_map[y][x] == 4 ){
              arrow_map[y][x] = LEFT; change = 1;
            }
          }       // else
        }         // if( arrow_map < 4 )
      }           // for( ii )


    /* no recuerdo como revisa este ciclo
    for( i = 0; i < MAP_HSIZE; i++ )
      for( ii = 0; ii < MAP_WSIZE; ii++ ){
        if( arrow_map[i][ii] < 4 ){
          for( int dir = 0; dir < 4; dir++ ){
            y = i  + dy[dir];
            x = ii + dx[dir];
            if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
              // out of limits
            } else {
              if( map[y][x] && arrow_map[y][x] == 4 )
                switch( dir ){
                case RIGHT: arrow_map[y][x] = LEFT ; change = 1; break;
                case UP   : arrow_map[y][x] = DOWN ; change = 1; break;
                case LEFT : arrow_map[y][x] = RIGHT; change = 1; break;
                case DOWN : arrow_map[y][x] = UP   ; change = 1; break;
                } // switch
            }     // else
          }       // for( dir )
        }         // if( arrow_map < 4 )
      }           // for( ii )

    y = MAP_HSIZE - 1 -  i;
    x = MAP_WSIZE - 1 - ii;

    if( arrow_map[y][x] < 4 ){
      for( int dir = 0; dir < 4; dir++ ){
        y = y + dy[dir];
        x = x + dx[dir];
        if( x < 0 || y < 0 || x >= MAP_WSIZE || y >= MAP_HSIZE ){
          // out of limits
        } else {
          if( map[y][x] && arrow_map[y][x] == 4 )
            switch( dir ){
            case RIGHT: arrow_map[y][x] = LEFT ; change = 1; break;
            case UP   : arrow_map[y][x] = DOWN ; change = 1; break;
            case LEFT : arrow_map[y][x] = RIGHT; change = 1; break;
            case DOWN : arrow_map[y][x] = UP   ; change = 1; break;
            } // switch
        }     // else
      }       // for( dir )
    }         // if( arrow_map < 4 )
    */


  } // while( change )

  return 0;
}

////////////////////////////////////////////////// JUEGO

// dibuja al jugador muerto
void kill_player( char map[MAP_HSIZE][MAP_WSIZE], gps player ){
  map[ player.y ][ player.x ] = BLOOD;

  int i;
  for( i = 1; i < 3; i++ ){
    change_chmap( map, player.y    , player.x - i, BLOOD, 0    );  //   +
    change_chmap( map, player.y    , player.x + i, BLOOD, 0    );  //  +Z+
    change_chmap( map, player.y - i, player.x    , BLOOD, 0    );  //   + 
    change_chmap( map, player.y + i, player.x    , BLOOD, 0    );
    change_chmap( map, player.y - i, player.x - i, BLOOD, DIRT );  //  + +
    change_chmap( map, player.y - i, player.x + i, BLOOD, DIRT );  //   Z
    change_chmap( map, player.y + i, player.x - i, BLOOD, DIRT );  //  + +
    change_chmap( map, player.y + i, player.x + i, BLOOD, DIRT );
    draw_map( map, MAP_Y, MAP_X );
    refresh();
    SLEEP( 0, 400000000 );
  }

  flushinp();
}

// verifica que se pueda realizar a el movimiento del jugador y lo aplica
int move_player( char map[MAP_HSIZE][MAP_WSIZE], gps* player, enum MV move ){
  int iy = player->y + dy[ move ];
  int ix = player->x + dx[ move ];
  if( ix < 0 || iy < 0 || ix >= MAP_WSIZE || iy >= MAP_HSIZE ) return 0;

  char obj = map[ iy ][ ix ];

  if( obj == MONSTER ){
    return -1;
  }

  if( obj == EMPTY || obj == DIRT ){
    map[ player->y ][ player->x ] = EMPTY;
    player->y = iy;
    player->x = ix;
    map[ iy ][ ix ] = PLAYER;
    return 1;
  } else return 0;
}

// carga desde fichero y establece la ubicacion del jugador
void load_level( char map[MAP_HSIZE][MAP_WSIZE], gps* player ){
  load_map( "maze", map );
  player->y = -1;

  int i, ii;
  for( i = 0; i < MAP_HSIZE; i++ )
    for( ii = 0; ii < MAP_WSIZE; ii++ ){
      if( map[ i ][ ii ] == PLAYER ){ 
        player->y = i;
        player->x = ii;
      }
    }

  if( player->y == -1 ) map[ player->y = 0 ][ player->x = 0 ] = PLAYER;
}

void load_player( char map[MAP_HSIZE][MAP_WSIZE], gps* player ){
  player->y = -1;

  int i, ii;
  for( i = 0; i < MAP_HSIZE; i++ )
    for( ii = 0; ii < MAP_WSIZE; ii++ ){
      if( map[ i ][ ii ] == PLAYER ){ 
        player->y = i;
        player->x = ii;
      }
    }

  if( player->y == -1 ) map[ player->y = 0 ][ player->x = 0 ] = PLAYER;
}

// reliza el movimiento de los mostruos mediante el mapa de flechas y el del laberinto
int do_the_monster_dance( char map[MAP_HSIZE][MAP_WSIZE], gps player ) {
  // para el mapa de flechas
  char path_map[ MAP_HSIZE ][ MAP_WSIZE ];
  memset( path_map, 4, sizeof(char) * MAP_HSIZE * MAP_WSIZE );
  // para marcar si es visitable o no y mandar a la funcion path_find
  char IO_map[MAP_HSIZE][MAP_WSIZE];
  memset( IO_map, 1, sizeof(char) * MAP_HSIZE * MAP_WSIZE );
  // para almacenar la ubicacion inicial de los mountros
  char monster_map[MAP_HSIZE][MAP_WSIZE];
  memset( monster_map, 0, sizeof(char) * MAP_HSIZE * MAP_WSIZE );

  // establece los elementos de los mapas
  int iy, ix; 
  for( iy = 0; iy < MAP_HSIZE; iy++ ) 
    for( ix = 0; ix < MAP_WSIZE; ix++ ){
      if( map[iy][ix] == DIRT || map[iy][ix] == WALL )
        IO_map[iy][ix] = 0;
      if( map[iy][ix] == MONSTER )
        monster_map[iy][ix] = 1;
    }

  path_find( IO_map, path_map, player );

  // realiza una busqueda de los mostruos
  gps next;
  for( iy = 0; iy < MAP_HSIZE; iy++ ) 
    for( ix = 0; ix < MAP_WSIZE; ix++ )
      // si es mostruo
      if( monster_map[iy][ix] ){
        // si hay una flecha hacia la posicion de jugador
        if(  path_map[iy][ix] < 4 ){
          // la nueva ubicacion del mostruo
          next.y = iy + dy[ (int)path_map[iy][ix] ];
          next.x = ix + dx[ (int)path_map[iy][ix] ];

          // si en la nueva ubicacion no hay un  mostruo
          if( map[ next.y ][ next.x ] != MONSTER ){
            // si esta el jugador confirma la muerte
            if( next.y == player.y && next.x == player.x  ){
              return 1;
            } else { // sino realiza el movimiento
              map[ iy     ][ ix     ] = EMPTY;
              map[ next.y ][ next.x ] = MONSTER;
            }
          }
        } else { // do the monster dance ** si no hay un camino al jugador
          int dir = rand() % 4;

          next.y = iy + dy[dir];
          next.x = ix + dx[dir];
          if( next.x < 0 || next.y < 0 || next.x >= MAP_WSIZE || next.y >= MAP_HSIZE ){
            // out of limits
          } else {
            if( map[ next.y ][ next.x ] == EMPTY ){
              map[ iy     ][ ix     ]    = EMPTY;
              map[ next.y ][ next.x ]    = MONSTER;
            }
          }

        } // else
      }   // if( moster_map )

  return 0;
}

// ayuda para el editor
void editor_help(){
  WINDOW* helpwin = newwin(  18, 37, ( HEIGHT - 16  ) / 2, ( WIDTH - 37 ) / 2 );
  wbkgd( helpwin, COLOR_PAIR( YB ) );
  box( helpwin, 0, 0 );
  mvwaddstr( helpwin,  1, 1, "----------  EDITOR HELP  ----------" );
  mvwaddstr( helpwin,  2, 1, "      h -> ESTE MENU               " );
  mvwaddstr( helpwin,  3, 1, "      g -> GENERAR LABERINTO       " );
  mvwaddstr( helpwin,  4, 1, "      e -> CURSOR ES CAMINO        " );
  mvwaddstr( helpwin,  5, 1, "      w -> CURSOR ES MURO          " );
  mvwaddstr( helpwin,  6, 1, "      d -> CURSOR ES MURO BLANDO   " );
  mvwaddstr( helpwin,  7, 1, "      z -> CURSOR ES Z             " );
  mvwaddstr( helpwin,  8, 1, "      m -> CURSOR ES MONSTRUO      " );
  mvwaddstr( helpwin,  9, 1, "      a -> TODO EL MAPA = CURSOR   " );
  mvwaddstr( helpwin, 10, 1, "espacio -> POSICION     = CURSOR   " );
  mvwaddstr( helpwin, 11, 1, "  enter -> POSICION     = CURSOR   " );
  mvwaddstr( helpwin, 12, 1, "      l -> BLOQ/DESBLOQ POS = CUR  " );
  mvwaddstr( helpwin, 13, 1, "flechas -> MOVER CURSOR            " );
  mvwaddstr( helpwin, 14, 1, "      s -> SALVAR LAVERINTO        " );
  mvwaddstr( helpwin, 15, 1, "      r -> RECARGAR LAVERINTO      " );
  mvwaddstr( helpwin, 16, 1, "      q -> JUEGO                   " );

  wrefresh( helpwin );
  nodelay( stdscr, FALSE );
  getch();
  nodelay( stdscr, TRUE );
  delwin( helpwin );
}

// el editor
void editor( char map[][ MAP_WSIZE ], gps * player ){
  clear();
  nodelay( stdscr, TRUE );

  int lock = -1;
  int obj = EMPTY;
  int curs_x = 0, curs_y = 0;

  int run = 1;
  while( run ){
    switch( tolower( getch() ) ){
    case KEY_UP   : curs_y--      ; break;
    case KEY_DOWN : curs_y++      ; break;
    case KEY_LEFT : curs_x--      ; break;
    case KEY_RIGHT: curs_x++      ; break;

    case 'e' : obj = EMPTY            ; break;
    case 'd' : obj = DIRT             ; break;
    case 'w' : obj = WALL             ; break;
    case 'm' : obj = MONSTER          ; break;
    case 'z' : obj = PLAYER           ; break;
    case 'l' : lock *= -1             ; break;
    case 'h' : editor_help()          ; break;
    case 'q' : run = 0                ; break;
    case 27  : run = 0                ; break;
    case 's' : save(  map )           ; break;
    case 'g' : generate( map )        ; break;
    case 'r' : load_map( "maze", map ); break;
    case 'a' : 
      if( obj != PLAYER )
        memset( map, obj, sizeof(char) * MAP_HSIZE * MAP_WSIZE );
      break;
    case 10  :
      if( obj == PLAYER ){
        map[ player->y ][ player->x ] = EMPTY;
        map[ player->y = curs_y ][ player->x = curs_x ] = PLAYER;
      } else map[curs_y][curs_x] = obj;
      break;
    case ' ' :
      if( obj == PLAYER ){
        map[ player->y ][ player->x ] = EMPTY;
        map[ player->y = curs_y ][ player->x = curs_x ] = PLAYER;
      } else map[curs_y][curs_x] = obj;
      curs_x++;
      break;
    }

    if( curs_y >= MAP_HSIZE ) curs_y = 0;
    if( curs_y <  0         ) curs_y = MAP_HSIZE - 1;
    if( curs_x >= MAP_WSIZE ) curs_x = 0;
    if( curs_x <  0         ) curs_x = MAP_WSIZE - 1;

    if( lock == 1 ){
      if( obj == PLAYER ){
        map[ player->y ][ player->x ] = EMPTY;
        map[ player->y = curs_y ][ player->x = curs_x ] = PLAYER;
      } else map[curs_y][curs_x] = obj;
    }

    draw_map( map, MAP_Y, MAP_X );
    draw_chwin( stdscr, curs_y + MAP_Y, curs_x + MAP_X, obj | COLOR_PAIR( BW ) );
    refresh();
    SLEEP( 0, HZ );
  }
}

// ayuda para el juego
void game_help(){
  WINDOW* helpwin = newwin(  9, 35, ( HEIGHT - 9  ) / 2, ( WIDTH - 35 ) / 2 );
  wbkgd( helpwin, COLOR_PAIR( YB ) );
  box( helpwin, 0, 0 );
  mvwaddstr( helpwin,  1, 1, "----------  GAME HELP  ----------" ); 
  mvwaddstr( helpwin,  2, 1, "      h -> ESTE MENU             " ); 
  mvwaddstr( helpwin,  3, 1, "      p -> PAUSA / CONTINUAR     " ); 
  mvwaddstr( helpwin,  4, 1, "      e -> EDITOR                " );
  mvwaddstr( helpwin,  5, 1, "      r -> VOLVER A INICIAR      " ); 
  mvwaddstr( helpwin,  6, 1, "flechas -> MOVER                 " ); 
  mvwaddstr( helpwin,  7, 1, "      q -> REGRESAR              " ); 

  wrefresh( helpwin );
  nodelay( stdscr, FALSE );
  getch();
  nodelay( stdscr, TRUE );
  delwin( helpwin );
}

// el juego
int game(){
  clear();

  char map[ MAP_HSIZE ][ MAP_WSIZE ];
  load_map( "maze", map );

  int die = 0;
  gps player;

  load_level( map, &player );

  nodelay( stdscr, TRUE );

  int run = 1;
  int count = 0;
  while( run ){
    count++;
    switch( tolower( getch() ) ){
    case KEY_UP   : if( move_player( map, &player, UP    ) == -1 ) die = 1; break;
    case KEY_DOWN : if( move_player( map, &player, DOWN  ) == -1 ) die = 1; break;
    case KEY_LEFT : if( move_player( map, &player, LEFT  ) == -1 ) die = 1; break;
    case KEY_RIGHT: if( move_player( map, &player, RIGHT ) == -1 ) die = 1; break;
    case 'r'      : load_level( map, &player )                            ; break;
    case 'e'      : editor( map, &player ); load_player( map, &player )   ; break;
    case 'h'      : game_help()                                           ; break;
    case 27       : run = false                                           ; break;
    case 'q'      : run = false                                           ; break;
    case 'p'      : nodelay( stdscr, FALSE ); getch(); nodelay( stdscr, TRUE )  ; break;
    default       :                                                               break;
    } // switch( ch )

    if( count >= 4 ){
      if( do_the_monster_dance( map, player ) ) die = 1;
      count = 0;
    }
    if( die ){
      kill_player( map, player );
      return 0    ;
    }

    draw_map( map, MAP_Y, MAP_X );
    refresh();
    SLEEP( 0, HZ );
  }   // while( run )

  nodelay( stdscr, FALSE );
  return 0;
}

////////////////////////////////////////////////// 

int main( int argc, char** argv ){
  start_curses();
  start_ck();

  terminalsize();
  nasciiboy();

  while( menu() ){
    game();
  }

  end_curses();
  return 0;
}
