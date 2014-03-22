#ifndef COMMON_H
#define COMMON_H

#define GLOBAL_WIDTH 80
#define GLOBAL_HEIGHT 25 

#define VERSION "v 5000"
#define HZ        25000000

#define MAP_WSIZE 80
#define MAP_HSIZE 23
#define MAP_X      0
#define MAP_Y      1

#define EMPTY   ' '
#define DIRT    '#'
#define STONE   'O'
#define WALL    ':'
#define DIAMOND '*'
#define MONEY   '$'
#define PLAYER  'Z'
#define BOMB    '@'
#define BOMBPK  '%'
#define MONSTER 'M'
#define BLOOD   '+'

#define POINTS_DIAMOND   10
#define POINTS_MONEY     100
#define NUM_BOMBPK       3

enum MV { NONE, DOWN, UP, LEFT, RIGHT };

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


#endif  // COMMON_H
