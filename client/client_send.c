#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdint.h>
#include "err.h"

#define GET 100
#define SET 200
#define UNKNOWN -1

typedef struct {
    int code;
    int len;
    char data[255];
} request;

void readStdin();
int commandToNumber(char *command);
void sendRequest(int socket, request *req);
void parseRequest(char *line, request *req);

int main() {
    readStdin();

    return 0;
}

int commandToNumber(char *command) {
    if(strcmp("get", command) == 0) {
        return GET;
    } else if(strcmp("set", command) == 0) {
        return SET;
    } else {
        fprintf(stderr, "Wrong command\n");
        return UNKNOWN;
    }
}

void sendRequest(int socket, request *req) {

}

void parseRequest(char *buffer, request *req) {
    char command[4];
    char data[257];
    int resource = -1;
    int numRead;

    numRead = sscanf(buffer, "%3s%*[ ]%d%256[^\n]s", command, &resource, data);

    if(resource < 0 || resource > 33) {
        fprintf(stderr, "Invalid resource: %d\n", resource);
        req->code = UNKNOWN;
        return;
    }

    int code = commandToNumber(command);

    if(code == UNKNOWN) {
        fprintf(stderr, "Command number < 0\n");
        req->code = UNKNOWN;
        return;
    }

    if(!(code == 100 && numRead == 2) && !(code == 200 && numRead == 3 && data[0] == ' ')) {
        fprintf(stderr, "Invalid command: %d, %d\n", code, numRead);
        req->code = UNKNOWN;
        return;
    }

    req->code = code + resource;
    req->len = strlen(data) - 1;
    strncpy(req->data, data+1, 255);
}

void readStdin() {
    const int size = 255 + 7 + 1;
    char buffer[size];

    while(fgets(buffer, size, stdin) != NULL) {
    //   char command[4] = {0};
    //   int resource = -1;
    //   char data[257] = {0};

      if (buffer[strlen(buffer)-1] != '\n') {
        char ch;
        while (((ch = getchar()) != '\n') && (ch != EOF)) {}
      }

      request req;

      parseRequest(buffer, &req);

      if(req.code == UNKNOWN) {
          continue;
      }

      printf("to Send> %d %d <%s>\n", req.code, req.len, req.data);
    }
}
