#include "networking.h"

int main(int argc, char *argv[]) {
  char* IP = "127.0.0.1";
  if(argc > 1) IP = argv[1];

  int server_socket = client_tcp_handshake(IP);
  printf("Connected to server! Waiting for game...\n");

  fd_set read_fds;
  char buff[BUFFER_SIZE];

  while(1){
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    FD_SET(server_socket, &read_fds);

    if (select(server_socket + 1, &read_fds, NULL, NULL, NULL) == -1) {
      perror("select");
      exit(1);
    }

    // Message from Server
    if(FD_ISSET(server_socket, &read_fds)){
      memset(buff, 0, sizeof(buff));
      int bytes = recv(server_socket, buff, sizeof(buff) - 1, 0);

      if(bytes <= 0){
        printf("\nDisconnected from server.\n");
        exit(0);
      }
      printf("%s", buff);
      fflush(stdout); // Ensure text appears immediately
    }

    // Input from User
    if(FD_ISSET(STDIN_FILENO, &read_fds)){
      memset(buff, 0, sizeof(buff));
      if(fgets(buff, sizeof(buff), stdin)){
        buff[strcspn(buff, "\r\n")] = 0;
        send(server_socket, buff, strlen(buff), 0);
      }
    }
  }
}