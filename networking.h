#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/select.h>

#ifndef NETWORKING_H
#define NETWORKING_H
#define PORT "19230"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

typedef struct {
    int fd;// The socket  (ID)
    int lives; // 3 lives
    int in_game; // Is this slot currently taken?
    char name[20];
} player;

typedef struct {
    player players[MAX_CLIENTS];
    int num_players;
} game_state;

void err(int i, char*message);
int server_setup();
int client_tcp_handshake(char*server_address);
void server_tcp_handshake(int listen_socket, fd_set *master, int *fdmax, game_state *game);
#endif
