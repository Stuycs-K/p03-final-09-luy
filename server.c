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
    if(game->players[i].in_game){
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
  
  printf("[SERVER]: Loaded %d words into dictionary.\n", game->dict_count);
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

    close(client_socket);
    FD_CLR(client_socket, master);
    p->in_game = 0;
    game->num_players--;
    return 0;
  }

  buffer[strcspn(buffer, "\n")] = 0;

  char game_msg[BUFFER_SIZE];
  snprintf(game_msg, sizeof(game_msg), "[GAME]: %s says %s", p->name, buffer);

  printf("Log: %s\n", game_msg);
  broadcast(game, game_msg);
  return 1;
}



int main(int argc, char *argv[] ) {
  fd_set read_fds;
  fd_set master;
  int fd_max;
  game_state game;

  memset(&game, 0, sizeof(game_state));
  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  int listen_socket = server_setup();
  //  int client_socket = 0;
  // int client_socket = server_tcp_handshake(listen_socket);
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
