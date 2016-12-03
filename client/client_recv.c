#include <stdio.h>
#include <sys/socket.h>
#include "err.h"


void receiveAndPrint(int socket) {
  int bytesRead;
  const int size = 100;
  char buffer[size];

  while(1) {
    bytesRead = recv(socket, buffer, size - 1, 0);
    if(bytesRead < 0) {
      dieWithError("Error occured receiving messages from server");
    } else if (bytesRead == 0) {
      dieWithError("socket closed");
    } else {
      buffer[size - 1] = '\0';
      printf("%s", buffer);
    }
  }

}
