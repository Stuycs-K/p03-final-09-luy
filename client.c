#include "networking.h"

void clientLogic(int server_socket){
  char msg[BUFFER_SIZE];
  char response[BUFFER_SIZE];
  memset(msg, 0, sizeof(msg));
  memset(response, 0, sizeof(response));

  printf("Enter a message:\n");
  if((fgets(msg, sizeof(msg), stdin)) == NULL){
    exit(0);
  }

  // Send to server
  send(server_socket, msg, strlen(msg), 0);

  // Wait for response
  if (recv(server_socket, response, sizeof(response), 0) > 0) {
      printf("Server says: %s\n", response);
  }
}

int main(int argc, char *argv[] ) {
  char* IP = "127.0.0.1";
  if(argc>1){
    IP=argv[1];
  }
  int server_socket = client_tcp_handshake(IP);
  printf("client connected.\n");
  while(1){
    clientLogic(server_socket);
  }
  close(server_socket);
  printf("Client closed\n");
}
