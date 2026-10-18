#include <arpa/inet.h>
#include <sys/socket.h>


#ifndef COMMON_H
#define COMMON_H

// Define any structs you may find helpful here.
#define BOARD_ROWS 6 
#define BOARD_COLS 7 
#define MAX_GAMES 100

struct board { 
    int cells[BOARD_ROWS * BOARD_COLS]; 
};

#endif