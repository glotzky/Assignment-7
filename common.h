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


struct game {
    int gameId;
    int current_player;
    struct board board;
};

struct game_state {
    struct game game;
    struct sockaddr_in clients[2];
    int player_count;
};

struct gameDB {
    struct game_state games[MAX_GAMES];
    int next_game_id;
};

enum message_type {
    MESSAGE_CREATE = 1,
    MESSAGE_JOIN,
    MESSAGE_MOVE,
    MESSAGE_WAIT,
    MESSAGE_UPDATE,
    MESSAGE_ERROR,
    MESSAGE_INVALID_MOVE
};

struct message {
    int type;
    int game_id;
    int player;
    int column;
    int winner;
    int tie;
    struct board board;
};

#endif