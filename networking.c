#define _DEFAULT_SOURCE
#include "networking.h"
#include <netdb.h>

/*Create and bind a socket.
* Place the socket in a listening state.
* returns the socket descriptor
*/
int server_setup() {
  printf("Hi, welcome to MEGASYMBOLDETHWORDS.\n\n");
  usleep(300);
  printf("\tTo initalize a game please make sure you run more client[s]!\n\n");
  usleep(90000);
  printf("\tAttempting to run server setup now\n\n");

  int stat;
  struct addrinfo hints, *results; // use stack-allocated hints
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM; //TCP socket
  hints.ai_flags = AI_PASSIVE;
  // obtains server information
  if ((stat = getaddrinfo(NULL, PORT, &hints, &results)) != 0) {
    err(stat, "[SERVER SETUP]: addrinfo error!");
    exit(1);
  }

  //create the socket
  int listen_fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
  err(listen_fd, "[SERVER SETUP]: socket error! This is likely because getaddrinfo is off somewhere.");

  // restart target port 7142 just in case
  int yes = 1;
  if ( setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1 ) {
      printf("[SERVER SETUP]: sockopt error. could not restart port!\n");
      printf("%s\n",strerror(errno));
      exit(-1);
  }

  //bound to port 7142 btw
  int binder = bind(listen_fd, results->ai_addr, results->ai_addrlen);
  if (binder == -1) {
    err(binder, "[SERVER SETUP]: bind error! Is there already an existing server?");
    exit(1);
  }
  printf("[SERVER SETUP]: Bind complete\n");

  //now listening
  listen(listen_fd, 2);
  printf("[SERVER SETUP]: Server will be listening for connections on PORT: %s\n", PORT);


  freeaddrinfo(results);
  return listen_fd;
}

/*Accept a connection from a client
 *return the socket descriptor for the new socket connected to the client
 *blocks until connection is made.
 *ALSO prints current game state for client
 */
void server_tcp_handshake(int listen_socket, fd_set *master, int *fdmax, game_state *game){
  struct sockaddr_storage client_addr;
  socklen_t addr_len = sizeof(client_addr);
  // accept connection with new private socket
  int client_socket = accept(listen_socket, (struct sockaddr*)&client_addr, &addr_len);


  if (client_socket == -1) {
    err(client_socket, "[SERVER]: Could not accept new client!");
  }else{ // update fd max to this new socket
    FD_SET(client_socket, master);
    if(client_socket > *fdmax){
      *fdmax = client_socket;}
  }

  int i;
  int slot_found = 0;

  //let this socket into game?
  for(i = 0; i < MAX_CLIENTS; i++) {
    if (game->players[i].in_game == 0) {
      // save socket id, setup player stats, and update global stats
      game->players[i].fd = client_socket;
      game->players[i].lives = 3;
      game->players[i].in_game = 1;
      game->num_players++;
      sprintf(game->players[i].name, "Player %d", i); // Assign simple name

      printf("[SERVER]: New connection ok! %s added on socket %d\n", game->players[i].name, client_socket);

      slot_found = 1;

      char welcome[BUFFER_SIZE];
      snprintf(welcome, sizeof(welcome),
      "Welcome to MEGASYMBOLDETHWORDS!\n\n\tYou are %s!\n It is VERY important to remember this!\n\nCurrent Prompt: A [4] Letter word starting with:  '%s' \nCurrent Turn: %s\n",
      game->players[i].name,
      game->current_prompt,
      game->players[game->turn_index].name);

      if(strcasecmp(game->players[i].name, game->players[game->turn_index].name) == 0){
        strcat(welcome, "\n\nIT'S YOUR TURN!!\n");
      }

      send(client_socket, welcome, strlen(welcome), 0);
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

  int stat;
  if ((stat = getaddrinfo(server_address, PORT, &hints, &results)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(stat));
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
    break;
  }

  if (server_fd == -1) {
    printf("Connection failed.\n");
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
