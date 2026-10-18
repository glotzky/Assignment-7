#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "common.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096


// prints the 7x6 flattened board array in the correct format
void print_board(int board[]) {
 
    printf("\n");
    for (int col = 0; col < 7; col++)
        printf(" %d", col);
    printf("\n");
 
    for (int row = 0; row < 6; row++) {
        printf("|");
        for (int col = 0; col < 7; col++) {
            int cell = board[row * 7 + col];
            printf("%c|", cell == 0 ? 'X' : cell == 1 ? 'O' : '.');
        }
        printf("\n");
    }
    printf("\n");
}

void print_game(struct message* m) {
    printf("\nCurrent Game\n");
    printf("---------------------\n");
    printf("GameID: %d\n", m->game_id);
    print_board(m->board.cells);
}


int main() {
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);


    printf("Connect 4\n");
    printf("  1) Create a new game\n");
    printf("  2) Join a game\n");
    printf("> ");
    int choice = 0;
    scanf("%d", &choice);

    // TODO: implement client logic

    struct message m;
    memset(&m, 0, sizeof(m));
    m.type = choice == 1 ? MESSAGE_CREATE : MESSAGE_JOIN;
    if (choice == 2) {
        printf("Game ID? >");
        scanf("%d", &m.game_id);
    }
    sendto(sock_fd, &m, sizeof(m), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    recvfrom(sock_fd, &m, sizeof(m), 0, NULL, NULL);
    if (m.type == MESSAGE_ERROR) {
        printf("Error: game does not exist\n");
        close(sock_fd);
        return 1;
    }

    int player = choice - 1;
    print_game(&m);
    printf("waiting for other player...\n");

    while (1) {
        recvfrom(sock_fd, &m, sizeof(m), 0, NULL, NULL);
        print_game(&m);
        if (m.winner >= 0) {
            printf(m.winner == player ? "YOU WIN!\n" : "YOU LOSE!\n");
            break;
        }
        if (m.tie) {
            printf("TIE\n");
            break;
        }
        if (m.player != player) {
            printf("waiting for other player...\n");
            continue;
        }
        printf("\nInput Move Row: > ");
        scanf("%d", &m.column);
        m.type = MESSAGE_MOVE;
        sendto(sock_fd, &m, sizeof(m), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    }
    
    close(sock_fd);

    return 0;
}