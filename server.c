#define _DEFAULT_SOURCE
#include "networking.h"

void send_msg(int fd, char *str) {
    send(fd, str, strlen(str), 0);
}

void broadcast(game_state *game, char *msg){
  printf("[BROADCAST]: %s", msg);
  for(int i = 0; i < MAX_CLIENTS; i++){
    if(game->players[i].in_game){
      send_msg(game->players[i].fd, msg);
    }
  }
}

player* find_player(int socket, game_state *game) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (game->players[i].in_game && game->players[i].fd == socket) {
      return &game->players[i];
    }
  }
  return NULL;
}

void load_dictionary(game_state *game) {
  FILE *fd = fopen("words.txt", "r");
  if (!fd) {
    perror("Could not open words.txt");
    exit(1);
  }

  game->dictionary = malloc(sizeof(char*) * DICT_SIZE);
  game->dict_count = 0;

  char buffer[MAX_WORD_LEN];
  while (fgets(buffer, sizeof(buffer), fd) && game->dict_count < DICT_SIZE) {
    buffer[strcspn(buffer, "\r\n")] = 0;
    for(int i=0; buffer[i]; i++) buffer[i] = toupper(buffer[i]);

    game->dictionary[game->dict_count] = malloc(strlen(buffer) + 1);
    strcpy(game->dictionary[game->dict_count], buffer);
    game->dict_count++;
  }

  printf("[SERVER]: Loaded %d words.\n", game->dict_count);
  fclose(fd);
}

int is_word_in_dict(char *word, game_state *game) {
  for (int i = 0; i < game->dict_count; i++) {
    if (strcasecmp(word, game->dictionary[i]) == 0) return 1;
  }
  return 0;
}

int is_word_used(char *word, game_state *game) {
  for (int i = 0; i < game->used_count; i++) {
    if (strcasecmp(word, game->used_words[i]) == 0) return 1;
  }
  return 0;
}

void advance_turn(game_state *game) {
    int start_index = game->turn_index;
    while(1) {
        game->turn_index = (game->turn_index + 1) % MAX_CLIENTS;
        if (game->players[game->turn_index].in_game) break;
        if (game->turn_index == start_index) break;
    }
}

void disconnect(int client_socket, game_state *game){
  player *p = find_player(client_socket, game);
  if (p == NULL) return;

  char leaveMsg[BUFFER_SIZE];
  sprintf(leaveMsg, "\n-- %s has left the game! --\n", p->name);
  broadcast(game, leaveMsg);

  p->in_game = 0;
  game->num_players--;
  close(client_socket);

  if (p == &game->players[game->turn_index]) {
    advance_turn(game);
    char turn_msg[BUFFER_SIZE];
    sprintf(turn_msg, "\n[GAME]: Turn passed to %s! Prompt: %s\n", game->players[game->turn_index].name, game->current_prompt);
    broadcast(game, turn_msg);
  }
}



int subserver_logic(int client_socket, game_state *game, fd_set *master){
  player *p = find_player(client_socket, game);
  if (p == NULL) return 0;

  char buffer[BUFFER_SIZE];
  memset(buffer, 0, sizeof(buffer));
  int n = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

  if(n <= 0){
    disconnect(client_socket, game);
    FD_CLR(client_socket, master);
    return 0;
  }

  buffer[strcspn(buffer, "\r\n")] = 0;
  if (strlen(buffer) == 0) return 1;

  if (p != &game->players[game->turn_index]) {
    send_msg(client_socket, "[GAME]: It is not your turn! Wait.\n");
    return 1;
  }

  if (toupper(buffer[0]) != toupper(game->current_prompt[0])) {
    p->lives--;
    char msg[BUFFER_SIZE];
    sprintf(msg, "[GAME]: Wrong letter! Must start with '%s'. Lives: %d\n", game->current_prompt, p->lives);
    send_msg(client_socket, msg);

    if(p->lives < 1) {
        send_msg(client_socket, "YOU DIED.\n");
        disconnect(client_socket, game);
        FD_CLR(client_socket, master);
        return 0;
    }
    return 1;
  }

  if (is_word_used(buffer, game)) {
    send_msg(client_socket, "[GAME]: Word already used!\n");
    return 1;
  }

  if (!is_word_in_dict(buffer, game)) {
    p->lives--;
    send_msg(client_socket, "[GAME]: Not in dictionary!\n");
    if(p->lives < 1) {
        send_msg(client_socket, "YOU DIED.\n");
        disconnect(client_socket, game);
        FD_CLR(client_socket, master);
        return 0;
    }
    return 1;
  }

  char success_msg[BUFFER_SIZE * 2];
  sprintf(success_msg, "[GAME]: %s played '%s'!\n", p->name, buffer);
  broadcast(game, success_msg);

  strcpy(game->used_words[game->used_count], buffer);
  game->used_count++;

  game->current_prompt[0] = toupper(buffer[strlen(buffer)-1]);
  game->current_prompt[1] = '\0';

  advance_turn(game);

  char turn_msg[BUFFER_SIZE];
  sprintf(turn_msg, "\nNext Turn: %s | Prompt: A [4] Letter word starting with: '%s' \n", game->players[game->turn_index].name, game->current_prompt);
  broadcast(game, turn_msg);

  return 1;
}

int main() {
  fd_set read_fds, master;
  int fd_max;
  game_state game;

  memset(&game, 0, sizeof(game_state));
  strcpy(game.current_prompt, "A");
  game.turn_index = 0;
  load_dictionary(&game);

  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  int listen_socket = server_setup();
  FD_SET(listen_socket, &master);
  fd_max = listen_socket;

  struct timeval tv;
  time_t turn_start_time = time(NULL);
  int turn_limit = 9;

  while(1){
    read_fds = master;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    
    if(select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1){
      perror("select");
      exit(1);
    }

    for(int i = 0; i <= fd_max; i++){
      if (FD_ISSET(i, &read_fds)) {
        if(i == listen_socket){
          server_tcp_handshake(listen_socket, &master, &fd_max, &game);
        } else {
          subserver_logic(i, &game, &master);
        }
      }
    }
  }
  return 0;
}
