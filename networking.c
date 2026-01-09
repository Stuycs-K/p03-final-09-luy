#define _DEFAULT_SOURCE
#include "networking.h"
#include <netdb.h>

/*Create and bind a socket.
* Place the socket in a listening state.
* returns the socket descriptor
*/
int server_setup() {
  //setup structs for getaddrinfo
  int stat;
  struct addrinfo hints, *results; // use stack-allocated hints
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM; //TCP socket
  hints.ai_flags = AI_PASSIVE;
  if ((stat = getaddrinfo(NULL, PORT, &hints, &results)) != 0) {
    err(stat, "addrinfo error");
    exit(1);
  }

  //create the socket
  int listen_fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
  err(listen_fd, "socket error");

  int yes = 1;
  if ( setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1 ) {
      printf("sockopt  error\n");
      printf("%s\n",strerror(errno));
      exit(-1);
  }

  //bind the socket to address and port
  int binder = bind(listen_fd, results->ai_addr, results->ai_addrlen);
  if (binder == -1) {
    err(binder, "bind error");
    exit(1);
  }
  printf("bind complete\n");
  //set socket to listen state

  listen(listen_fd, 2);
  printf("server listening for connections on PORT: %s\n", PORT);

  //free the structs used by getaddrinfo
  freeaddrinfo(results);

  return listen_fd;
}

/*Accept a connection from a client
 *return the socket descriptor for the new socket connected to the client
 *blocks until connection is made.
 */
void server_tcp_handshake(int listen_socket, fd_set *master, int *fdmax, game_state *game){
  struct sockaddr_storage client_addr;
  socklen_t addr_len = sizeof(client_addr);
  int client_socket = accept(listen_socket, (struct sockaddr*)&client_addr, &addr_len);
  if (client_socket == -1) {
    err(client_socket, "accept error");
  }else{
    FD_SET(client_socket, master);
    if(client_socket > *fdmax){
      *fdmax = client_socket;}
  }

  int i;
  int slot_found = 0;
  for(i = 0; i < MAX_CLIENTS; i++) {
    if (game->players[i].in_game == 0) {
      game->players[i].fd = client_socket;
      game->players[i].lives = 3;
      game->players[i].in_game = 1;
      sprintf(game->players[i].name, "Player %d", i); // Assign simple name
      
      game->num_players++;
      printf("New connection: %s added on socket %d\n", game->players[i].name, client_socket);
      
      slot_found = 1;
      break;
    }
  }

  if (!slot_found) {
    printf("Server full! Dropping connection.\n");
    close(client_socket);
    FD_CLR(client_socket, master);
  }
}



/*Connect to the server
 *return the to_server socket descriptor
 *blocks until connection is made.*/
int client_tcp_handshake(char * server_address) {
  struct addrinfo hints, *results, *p;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  int stat;
  if ((stat = getaddrinfo(server_address, PORT, &hints, &results)) != 0) {
    fprintf(stderr, "client getaddrinfo failed: %s\n", gai_strerror(stat));
    exit(1);
  }

  int server_fd = -1;
  for (p = results; p != NULL; p = p->ai_next) {
    server_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (server_fd == -1) continue;
    if (connect(server_fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(server_fd);
      server_fd = -1;
      continue;
    }
    break; // connected
  }

  if (server_fd == -1) {
    fprintf(stderr, "failed to connect to %s\n. Is server present?\n", server_address);
    freeaddrinfo(results);
    exit(1);
  }

  freeaddrinfo(results);
  return server_fd;
}




void err(int i, char*message){
  if(i < 0){
	  printf("Error: %s - %s\n",message, strerror(errno));
  	exit(1);
  }
}
