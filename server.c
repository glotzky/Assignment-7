#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "game.h"
#include "save.h"

#define PORT 8080

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
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Message board server listening on port %d\n", PORT);

    //TODO: implement game server logic
    struct gameDB db;
    memset(&db, 0, sizeof(db));

    while (1) {
        struct message m;
        socklen_t len = sizeof(client_addr);
        recvfrom(sock_fd, &m, sizeof(m), 0, (struct sockaddr*)&client_addr, &len);

        struct game_state* s = &db.games[m.game_id];
        if (m.type == MESSAGE_CREATE) {
            s = &db.games[db.next_game_id];
            new_game(&s->game, db.next_game_id++);
            s->clients[0] = client_addr;
            fill_update(&m, &s->game);
            send_msg(sock_fd, &client_addr, &m, MESSAGE_WAIT);
        } else if (m.game_id < 0 || m.game_id >= db.next_game_id) {
            send_msg(sock_fd, &client_addr, &m, MESSAGE_ERROR);
        } else if (m.type == MESSAGE_JOIN) {
            s->clients[1] = client_addr;
            fill_update(&m, &s->game);
            send_msg(sock_fd, &client_addr, &m, MESSAGE_WAIT);
            send_msg(sock_fd, &s->clients[0], &m, MESSAGE_UPDATE);
        } else if (m.type == MESSAGE_MOVE) {
            int p = client_addr.sin_port == s->clients[1].sin_port;
            if (p == s->game.current_player && make_move(&s->game, p, m.column) == 0) {
                fill_update(&m, &s->game);
                send_msg(sock_fd, &s->clients[0], &m, MESSAGE_UPDATE);
                send_msg(sock_fd, &s->clients[1], &m, MESSAGE_UPDATE);
            }
        }
    }
    close(sock_fd);
    return 0;
}