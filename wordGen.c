#include "networking.h"


int main(){

  int wordList = open("/usr/share/dict/words", O_RDONLY, 0644);
  if(fd == -1){
    err();
  }
  int fd = open("/usr/share/dict/words", O_WRONLY | O_CREAT | O_TRUNC, 0644);




}
