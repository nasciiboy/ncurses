// to translate original leves to text leves
#include <stdio.h>

#define MAP_WSIZE 80
#define MAP_HSIZE 23

#define MAP_EMPTY   0
#define MAP_DIRT    1
#define MAP_STONE   2
#define MAP_DIAMOND 3
#define MAP_WALL    4
#define MAP_MONEY   5
#define MAP_PLAYER  9
#define MAP_BOMB    6
#define MAP_BOMBPK  7
#define MAP_MONSTER 8

#define CHR_EMPTY   ' '
#define CHR_DIRT    '#' 
#define CHR_STONE   'O' 
#define CHR_WALL    ':' 
#define CHR_DIAMOND '*' 
#define CHR_MONEY   '$' 
#define CHR_PLAYER  'Z' 
#define CHR_BOMB    '@' 
#define CHR_BOMBPK  '%' 
#define CHR_MONSTER 'M' 

int main(){
  FILE *in, *out;

  int i;
  char op[30];
  char ne[30];
  for( i = 1; i < 12; i++ ){
    sprintf( op, "org_levels/%02i", i );
    sprintf( ne, "%02i", i );
    in  = fopen( op, "r" );
    out = fopen( ne, "w" );

    char ch;

    int nl = 0;
    while( ( ch = fgetc( in ) ) != EOF ){
      switch( ch ){
      case MAP_EMPTY    : fputc(  CHR_EMPTY   , out ); break;
      case MAP_DIRT     : fputc(  CHR_DIRT    , out ); break;
      case MAP_STONE    : fputc(  CHR_STONE   , out ); break;
      case MAP_DIAMOND  : fputc(  CHR_DIAMOND , out ); break;
      case MAP_WALL     : fputc(  CHR_WALL    , out ); break;
      case MAP_MONEY    : fputc(  CHR_MONEY   , out ); break;
      case MAP_PLAYER   : fputc(  CHR_PLAYER  , out ); break;
      case MAP_BOMB     : fputc(  CHR_BOMB    , out ); break;
      case MAP_BOMBPK   : fputc(  CHR_BOMBPK  , out ); break;
      case MAP_MONSTER  : fputc(  CHR_MONSTER , out ); break;
      default           :                              break;
      }
      if( ++nl >= MAP_WSIZE ){
        fputc( '\n', out );
        nl = 0;
      }
    }

    fclose( in );
    fclose( out );
  }

  return 0;
}
