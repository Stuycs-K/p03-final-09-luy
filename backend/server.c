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

void subserver_logic(int client_socket){
  char ret[BUFFER_SIZE];
  memset(ret, 0, sizeof(ret));
  char cpy[BUFFER_SIZE];
  memset(cpy, 0, sizeof(cpy));


  ssize_t n = recv(client_socket, ret, sizeof(ret) - 1, 0);
  if(n < 1){
    printf("EXITING!\n");
    exit(0);
  }
  strcpy(cpy, ret);
  rotX(ret, 13);

  printf("'%s' becomes '%s'\n", cpy, ret);
}

int main(int argc, char *argv[] ) {
  int listen_socket = server_setup();


  int client_socket = server_tcp_handshake(listen_socket);
  while(1){
    subserver_logic(client_socket);
  }
  close(client_socket);
  printf("Socket closed\n");

}
