#include <stdio.h>
#include <sys/socket.h>
#include <stdint.h>
#include "err.h"
#include "commands.h"
#include "utils.h"

void receiveAndPrint(int socket);

int main() {
    return 0;
}

void receiveAndPrint(int socket) {
  response res = { UNKNOWN, 0, {0} };

  while(1) {
      if(!read_uint8(socket, &res.status)) {
          dieWithError("Error reading command\n");
      }

      if(!read_uint8(socket, &res.len)) {
          dieWithError("Error reading data length\n");
      }

      if(res.len <= 0) {
          printf("Received response from server: Status code: %d\n", res.status);
          continue;
      }

      if(!readString(socket, res.data, res.len)) {
          dieWithError("Error reading data\n");
      }

      printf("Received response from server: Status code: %d\n", res.status);
      printf("Data: %s\n", res.data);

      /*bytesRead = recv(socket, buffer, size - 1, 0);
      if(bytesRead < 0) {
          dieWithError("Error occured receiving messages from server");
      } else if (bytesRead == 0) {
          dieWithError("socket closed");
      } else {
          buffer[size - 1] = '\0';
          printf("%s", buffer);
      }*/
  }

}
