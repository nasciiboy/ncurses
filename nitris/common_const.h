#ifndef COMMON_CONST_H
#define COMMON_CONST_H

#define VERSION        "v0.10"
#define HEIGHT             25              // global
#define WIDTH              80              // global
#define PULSE         5000000              // delay de cada ciclo para peticion de teclado
#define DELAY_DOWN         10              // cada n ciclos bajar pieza
#define DELAY_BOTTOM       20              // cata n ciclos comprobar pieza
#define CH_LEV             20              // nimero de lineas para cambiar nivel

#define WBOARD_HEIGHT      20              // size board window
#define WBOARD_WIDTH       32              // size board window
#define WBOARD_X            3              // init x position
#define WBOARD_Y            3              // init y position

#define WPREV_HEIGHT        6              // size preview window
#define WPREV_WIDTH        12              // size preview window
#define WPREV_X            38              // init x position
#define WPREV_Y             3              // init y position

#define WSCORE_HEIGHT      13              // size score window
#define WSCORE_WIDTH       12              // size score window
#define WSCORE_X           38              // init x position
#define WSCORE_Y           10              // init y position

#define CASH               10              // x one line

#define BOARD_HEIGHT ( WBOARD_HEIGHT - 2 ) // size board 
#define BOARD_WIDTH  ( WBOARD_WIDTH  - 2 ) // size board


///////////////////// MOVES  ///////////////////////////////////////////////
enum { MOVE, LEFT, RIGHT, DOWN, BOTTOM, ROTATE_L, ROTATE_R };

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

#endif  // COMMON_CONST_H
