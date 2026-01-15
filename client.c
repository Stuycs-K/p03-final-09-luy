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

  fd_set read_fds;
  while(1){
    FD_ZERO(&read_fds);
    FD_SET(fileno(stdin), &read_fds); // select from user input and server
    FD_SET(server_socket, &read_fds);

    if (select(server_socket + 1, &read_fds, NULL, NULL, NULL) == -1) { // server is proly max, but if it breaks rip
      perror("select");
      exit(1);
    }

    // server messages
    if(FD_ISSET(server_socket, &read_fds)){
      char buff[BUFFER_SIZE];
      memset(buff, 0, sizeof(buff));

      int bytes = recv(server_socket, buff, sizeof(buff) - 1, 0);

      
      if(bytes <= 0){
        printf("You died!\n"); // should probably do like a last message to print stats or something before exit 0ing
        exit(0);
      }
      printf("\n%s\n", buff);
      fflush(stdout);
    }

    // user inputs
    if(FD_ISSET(STDIN_FILENO, &read_fds)){
      char buff[BUFFER_SIZE];
      memset(buff, 0, sizeof(buff));
      if(fgets(buff, sizeof(buff), stdin)){
        buff[strcspn(buff, "\r\n")] = 0;
        send(server_socket, buff, strlen(buff), 0);
      }
    }
  }
  close(server_socket);
}
