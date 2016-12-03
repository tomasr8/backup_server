#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include "err.h"


void readAndSend(int socket) {
  //int bytesRead;
  const int size = 255 + 7 + 1;
  char buffer[size];

  while(fgets(buffer, size, stdin) != NULL) {
    char command[4] = {0};
    int resource;
    char data[256] = {0};
    //fprintf(stderr, "Inputed: <%s>\n", buff);

    if (buffer[strlen(buffer)-1] != '\n') {
      char ch;
      while (((ch = getchar()) != '\n') && (ch != EOF)) {}
    }

/*    for(int i = 0; i < 3; i++) {
      command[i] = buffer[i];
    }
    command[4] = '\0';

    if(buffer[4] != ' ') {
      fprintf(stderr, "invalid command");
      continue;
    }

    for(int i = 0; i < 3; i++) {
      command[i] = buffer[i];
    }
    command[4] = '\0';*/

    int numRead;
    numRead = sscanf(buffer, "%3s %d %255s", command, resource, data);

    if (numRead < 2) {
      fprintf(stderr, "invalid command");
    }

    

    //numread = sscanf(buff, "%d %d\n", &a, &b);
    //fprintf(stderr, "sscanf read %d numbers\n", numread);
  }

}
