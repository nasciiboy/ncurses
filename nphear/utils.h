#ifndef UTILS_H
#define UTILS_H


void start_curses();
void end_curses();
void start_ck();
void nasciiboy();
void min_size( int y, int x );
int getopt( int argc, char * const argv[], const char *optchar );
int msgbox( const char* msg );


#endif  // UTILS_H
