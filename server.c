#include "networking.h"

void rotX(char *s, int x){
  int t = x % 26;
  if(t < 0){
    t += 26;
  }
  for(int i= 0; s[i] != '\0'; i++){
    if(islower(s[i])){
      s[i] = ((s[i] - 'a' + t) % 26) + 'a';
    }else if (isupper(s[i])){
      s[i] = ((s[i] - 'A' + t)  % 26) + 'A';
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


int subserver_logic(int client_socket, game_state *game){
  player *p = find_player(client_socket, game);
  if (p == NULL) return 0;

  char buffer[BUFFER_SIZE];
  memset(buffer, 0, sizeof(buffer));

  ssize_t n = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
  if(n <= 0){
    printf("%s disconnected.\n", p->name);
    p->in_game = 0;
    game->num_players--;
    return 0;
  }
  
  char cpy[BUFFER_SIZE];
  strcpy(cpy, buffer);
  
  rotX(buffer, 13);
  printf("Socket %d: '%s' becomes '%s'\n", client_socket, cpy, buffer);

  send(client_socket, buffer, strlen(buffer), 0);
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
          int status = subserver_logic(i, &game);
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
