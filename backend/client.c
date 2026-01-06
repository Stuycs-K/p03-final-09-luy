#include "networking.h"

void clientLogic(int server_socket){
  char msg[BUFFER_SIZE];
  memset(msg, 0, sizeof(msg));

  printf("Enter a message:\n");
  if((fgets(msg, sizeof(msg), stdin)) == NULL){
    printf("NULL\n");
    exit(0);
  };
  printf("recieved: %s", msg);

  int b_send;
  b_send = send(server_socket, msg, strlen(msg), 0);
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
