#include "networking.h"

/*
  Returns address of player struct associated with socket within *game
  If not found, returns NULL.
*/
player* find_player(int socket, game_state *game) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (game->players[i].in_game && game->players[i].fd == socket) {
      return &game->players[i];
    }
  }
  return NULL;
}

/*
  Sends *msg to all connect and playing clients in *game
*/
void broadcast(game_state *game, char *msg){
  for(int i =0; i<MAX_CLIENTS; i++){
    if(game->players[i].in_game && (game->players[i].fd == game->players[game->turn_index].fd)){
      send(game->players[i].fd, msg, strlen(msg), 0);
    }
  }
}
/*
  Load words.txt and adds to current *game
*/
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
    buffer[strcspn(buffer, "\n")] = 0;
    // allocate exact memory for this specific word and then strcpy it
    game->dictionary[game->dict_count] = malloc(strlen(buffer) + 1);
    strcpy(game->dictionary[game->dict_count], buffer);
    game->dict_count++;
  }

  printf("[SERVER]: Loaded %d 4-letter words into dictionary.\n", game->dict_count);
  fclose(fd);
}

/*
  If word existing in the **dictionary stored in *game, return 1 else return 0
*/

int is_word_in_dict(char *word, game_state *game) {
  for (int i = 0; i < game->dict_count; i++) {
    if (strcasecmp(word, game->dictionary[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

/*
  If word exists in **used_words stored in *game return 1 else return 0
*/
int is_word_used(char *word, game_state *game) {
  for (int i = 0; i < game->used_count; i++) {
    if (strcasecmp(word, game->used_words[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

int disconnect(int client_socket, game_state *game){
  player *p = find_player(client_socket, game);
  if (p == NULL) return 0;
  ssize_t n = send(client_socket, 0, 0, 0);
  printf("%s disconnected.\n", p->name);
  char leaveMsg[BUFFER_SIZE];
  sprintf(leaveMsg, "-- %s has died! --", p->name);
  broadcast(game, leaveMsg);

  p->in_game = 0;
  game->num_players--;
  return 0;
}

/*
  Subserver logic for SERVER
*/
int subserver_logic(int client_socket, game_state *game, fd_set *master){
  player *p = find_player(client_socket, game);
  if (p == NULL) return 0;

  char buffer[BUFFER_SIZE];
  memset(buffer, 0, sizeof(buffer));

  ssize_t n = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

  if(n <= 0){
    printf("%s disconnected.\n", p->name);
    char leaveMsg[BUFFER_SIZE];
    sprintf(leaveMsg, "-- %s has left the game! --", p->name);
    broadcast(game, leaveMsg);

    p->in_game = 0;
    game->num_players--;
    return 0;
  }

  // GAME LOGIC BELOW

  // check if this is the right player going
  if (p != &game->players[game->turn_index]) {
    char *msg = "[GAME]: It is not your turn!\n";
    send(client_socket, msg, strlen(msg), 0);
    return 1;
  }

  // check word if it starts with the prompt
  if (strncasecmp(buffer, game->current_prompt, strlen(game->current_prompt)) != 0) {
    p->lives--;
    char msg[BUFFER_SIZE];
    snprintf(msg, sizeof(msg), "[GAME]: Bad guess %s! Must start with '%s'!\n -1 lives, you now have %d lives!\n", p->name, game->current_prompt, p->lives);
    if(p->lives < 1){
      disconnect(client_socket, game);
    }
    else{
      send(client_socket, msg, strlen(msg), 0);
    }
    return 1;
  }

  // check word in dict or used?
  if (is_word_used(buffer, game)) {
    char *msg = "[GAME]: Word already used!\n";
    send(client_socket, msg, strlen(msg), 0);
    return 1;
  }

  if (!is_word_in_dict(buffer, game)) {
    p->lives--;
    char *msg = "[GAME]: Not a valid word!\n";
    if(p->lives < 1){
      disconnect(client_socket, game);
    }else{
      send(client_socket, msg, strlen(msg), 0);
    }
    return 1;
  }


  // broadcast success
  char success_msg[BUFFER_SIZE];
  snprintf(success_msg, sizeof(success_msg), "[GAME]: %s used '%s' correctly!\n", p->name, buffer);
  broadcast(game, success_msg);

  // add to used list
  strcpy(game->used_words[game->used_count], buffer);
  game->used_count++;

  // pass turn until next valid player
  int old_turn = game->turn_index;
  while(1) {
    game->turn_index = (game->turn_index + 1) % MAX_CLIENTS;
    if (game->players[game->turn_index].in_game) break;
  }

  char turn_msg[BUFFER_SIZE];
  snprintf(turn_msg, sizeof(turn_msg), "[GAME]: next turn: %s! Prompt: %s\n", game->players[game->turn_index].name, game->current_prompt);
  broadcast(game, turn_msg);

  return 1;
}



int main(int argc, char *argv[] ) {
  fd_set read_fds;
  fd_set master;
  int fd_max;
  game_state game;

  memset(&game, 0, sizeof(game_state));

  char *letters = "ABCEDFGHIJKLMNOPQRSTUVWXYZ";

  strcpy(game.current_prompt, "N");
  game.turn_index = 0;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  int listen_socket = server_setup();
  //  int client_socket = 0;
  // int client_socket = server_tcp_handshake(listen_socket);
  load_dictionary(&game);
  FD_SET(listen_socket, &master);
  fd_max = listen_socket;

  while(1){
    read_fds = master;
    int i = select(fd_max+1, &read_fds, NULL, NULL, NULL);
    if(i == -1){
      perror("select");
      exit(1);
    }
    for(int i =0; i<=fd_max; i++){
      if (FD_ISSET(i, &read_fds)) {
        if(i == listen_socket){
          printf("Found valid client\n");
          server_tcp_handshake(listen_socket, &master, &fd_max, &game);
        }else{
          int status = subserver_logic(i, &game, &master);
          if (status == 0) {
            printf("Client on socket %d disconnected\n", i);
            close(i);
            FD_CLR(i, &master);
          }
        }
      }
    }

  }
  return 0;

}
