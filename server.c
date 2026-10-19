#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "game.h"
#include "save.h"

#define PORT 8080
#define SAVE_PATH "savegame.dat"

void send_msg(int sock_fd, struct sockaddr_in* to, struct message* m, int type) {
    m->type = type;
    sendto(sock_fd, m, sizeof(*m), 0, (struct sockaddr*)to, sizeof(*to));
}

void fill_update(struct message* m, struct game* g) {
    m->game_id = g->gameId;
    m->player = g->current_player;
    m->winner = check_win(g);
    m->board = g->board;
    m->tie = m->winner < 0;
    for (int col = 0; col < BOARD_COLS; col++) {
        if (g->board.cells[col] == -1) {
            m->tie = 0;
        }
    }
}

int main(int argc, char* argv[]) {
    struct gameDB db;
    memset(&db, 0, sizeof(db));
    if (argc > 1 && strcmp(argv[1], "1") == 0 && loadgame(&db, SAVE_PATH) < 0) {
        perror("Failed to load saved games");
        return 1;
    }

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(sock_fd);
        return 1;
    }

    printf("Message board server listening on port %d\n", PORT);
    
    while (1) {
        struct message m;
        socklen_t len = sizeof(client_addr);
        ssize_t received = recvfrom(sock_fd, &m, sizeof(m), 0,
                                    (struct sockaddr*)&client_addr, &len);
        if (received < 0) {
            perror("recvfrom");
            continue;
        }
        if ((size_t)received != sizeof(m)) {
            continue;
        }

        if (m.type == MESSAGE_CREATE) {
            if (db.next_game_id >= MAX_GAMES) {
                send_msg(sock_fd, &client_addr, &m, MESSAGE_ERROR);
                continue;
            }
            struct game_state* s = &db.games[db.next_game_id];
            new_game(&s->game, db.next_game_id++);
            s->clients[0] = client_addr;
            s->player_count = 1;
            fill_update(&m, &s->game);
            send_msg(sock_fd, &client_addr, &m, MESSAGE_WAIT);
        } else if (m.game_id < 0 || m.game_id >= db.next_game_id ||
                   m.game_id >= MAX_GAMES) {
            send_msg(sock_fd, &client_addr, &m, MESSAGE_ERROR);
        } else if (m.type == MESSAGE_JOIN) {
            struct game_state* s = &db.games[m.game_id];
            s->clients[1] = client_addr;
            s->player_count = 2;
            fill_update(&m, &s->game);
            send_msg(sock_fd, &client_addr, &m, MESSAGE_WAIT);
            send_msg(sock_fd, &s->clients[0], &m, MESSAGE_UPDATE);
        } else if (m.type == MESSAGE_MOVE) {
            struct game_state* s = &db.games[m.game_id];
            int p = client_addr.sin_port == s->clients[1].sin_port;
            if (p == s->game.current_player && make_move(&s->game, p, m.column) == 0) {
                if (savegame(&db, SAVE_PATH) < 0) {
                    perror("Failed to save games");
                }
                fill_update(&m, &s->game);
                send_msg(sock_fd, &s->clients[0], &m, MESSAGE_UPDATE);
                send_msg(sock_fd, &s->clients[1], &m, MESSAGE_UPDATE);
            }
        }
    }
    close(sock_fd);
    return 0;
}