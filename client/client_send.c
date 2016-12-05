#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "err.h"   // dieWitherror()
#include "commands.h" // command definitions
#include "utils.h"

// typedef struct {
//     uint8_t cmd;
//     uint8_t res;
//     uint8_t len;
//     char data[255];
// } request;

void readStdin();
int toNumber(char *command);
bool sendRequest(int socket, request *req);
bool parseRequest(char *line, request *req);


int main() {
    readStdin();

    return 0;
}

bool sendRequest(int socket, request *req) {
    uint8_t num;

    num = htons(req->cmd);
    if(send(socket, &num, sizeof(uint8_t), 0) <= 0) {
        printf("Failed to send cmd\n");
        return false;
    }

    num = htons(req->res);
    if(send(socket, &num, sizeof(uint8_t), 0) <= 0) {
        printf("Failed to send resource\n");
        return false;
    }

    if(req->len == 0) {
        return true;
    }

    num = htons(req->len);
    if(send(socket, &num, sizeof(uint8_t), 0) <= 0) {
        printf("Failed to send data length\n");
        return false;
    }

    if(send(socket, req->data, req->len, 0) <= 0) {
        printf("Failed to send data\n");
        return false;
    }

    return true;
}

bool parseRequest(char *buffer, request *req) {
    char cmdStr[4] = {0};
    char data[257] = {0};
    int resource = -1;

    int numRead = sscanf(buffer, "%3s%*[ ]%d%256[^\n]s", cmdStr, &resource, data);

    if(resource < 0 || resource > 33) {
        fprintf(stderr, "Invalid resource: %d\n", resource);
        return false;
    }

    int cmd = toNumber(cmdStr);

    if(cmd == UNKNOWN) {
        fprintf(stderr, "Uknnown command\n");
        return false;
    }

    if(!(cmd == GET && numRead == 2) && !(cmd == SET && numRead == 3 && data[0] == ' ')) {
        fprintf(stderr, "Invalid command: %d, %d\n", cmd, numRead);
        return false;
    }

    //printf("data: <%s>", data);

    req->cmd = (uint8_t) cmd;
    req->res = (uint8_t) resource;
    req->len = (uint8_t) (strlen(data) > 0 ? strlen(data) - 1 : 0);
    strncpy(req->data, data+1, 255);

    return true;
}

void readStdin() {
    const int size = 255 + 7 + 1;
    char buffer[size];

    while(fgets(buffer, size, stdin) != NULL) {

      if (buffer[strlen(buffer)-1] != '\n') {
        char ch;
        while (((ch = getchar()) != '\n') && (ch != EOF)) {}
      }

      request req = { UNKNOWN, 0, 0, {0} };

      if(!parseRequest(buffer, &req)) {
          continue;
      }

      printf("to Send> %d %d %d<%s>\n", req.cmd, req.res, req.len, req.data);
    }
}


int toNumber(char *command) {
    if(strcmp("get", command) == 0) {
        return GET;
    } else if(strcmp("set", command) == 0) {
        return SET;
    } else {
        fprintf(stderr, "Wrong command\n");
        return UNKNOWN;
    }
}
