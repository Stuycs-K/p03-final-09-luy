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
#define PORT "7142"
#define BUFFER_SIZE 8192
#define MAX_CLIENTS 10
#define MAX_WORD_LEN 50
#define DICT_SIZE 150000
#define MAX_USED_WORDS 500

typedef struct {
    int fd;// The socket  (ID)
    int lives; // 3 lives
    int in_game; // Is this slot currently taken?

    char name[20];
} player;

typedef struct {
    player players[MAX_CLIENTS];
    int num_players;
    int turn_index; // just set order
    char current_prompt[2]; // current starting letters
    int dict_count;
    char **dictionary; // stores words.txt
    char used_words[MAX_USED_WORDS][MAX_WORD_LEN];
    int used_count;
} game_state;

void err(int i, char*message);
int server_setup();
int client_tcp_handshake(char*server_address);
void server_tcp_handshake(int listen_socket, fd_set *master, int *fdmax, game_state *game);
#endif
