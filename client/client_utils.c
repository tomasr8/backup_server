#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "commands.h"
#include "utils.h"
#include "client_utils.h"

bool receive_response(int socket, response *res) {

    if(!read_uint16(socket, &res->status)) {
        return false;
        //dieWithError("Error reading command\n");
    }

    if(!read_uint16(socket, &res->len)) {
        return false;
        //dieWithError("Error reading data length\n");
    }

    if(res->len == 0) {
        //printf("Received response from server: Status code: %d\n", res.status);
        return true;
    }

    if(!read_str(socket, res->data, res->len)) {
        return false;
        //dieWithError("Error reading data\n");
    }

      //printf("Received response from server: Status code: %d\n", res.status);
      //printf("Data: %s\n", res.data);
    return true;
}

bool send_request(int socket, request *req) {
    uint16_t num;

    num = htons(req->cmd);
    if(send(socket, &num, sizeof(uint16_t), 0) <= 0) {
        fprintf(stderr, "Failed to send cmd\n");
        return false;
    }

    num = htons(req->res);
    if(send(socket, &num, sizeof(uint16_t), 0) <= 0) {
        fprintf(stderr, "Failed to send resource\n");
        return false;
    }

    num = htons(req->len);
    if(send(socket, &num, sizeof(uint16_t), 0) <= 0) {
        fprintf(stderr, "Failed to send data length\n");
        return false;
    }

    if(req->len == 0) {
        return true;
    }

    if(send(socket, req->data, req->len, 0) <= 0) {
        fprintf(stderr, "Failed to send data\n");
        return false;
    }

    return true;
}

bool parse_line(char *buffer, request *req) {
    char cmdStr[4] = {0};
    char data[257] = {0};
    int resource = -1;

    int numRead = sscanf(buffer, "%3s%*[ ]%d%256[^\n]s", cmdStr, &resource, data);

    if(resource < 0 || resource > 32) {
        fprintf(stderr, "Invalid resource: %d\n", resource);
        return false;
    }

    int cmd = to_number(cmdStr);

    if(cmd == UNKNOWN) {
        fprintf(stderr, "Uknnown command\n");
        return false;
    }

    if(!(cmd == GET && numRead == 2) && !(cmd == SET && numRead == 3 && data[0] == ' ')) {
        fprintf(stderr, "Invalid command: %d, %d\n", cmd, numRead);
        return false;
    }

    req->cmd = (uint16_t) cmd;
    req->res = (uint16_t) resource;
    req->len = (uint16_t) (strlen(data) > 0 ? strlen(data) - 1 : 0);
    strncpy(req->data, data+1, 256); // data[0] is space separating resource and data

    return true;
}

int to_number(char *command) {
    if(strcmp("get", command) == 0) {
        return GET;
    } else if(strcmp("set", command) == 0) {
        return SET;
    } else {
        fprintf(stderr, "Wrong command\n");
        return UNKNOWN;
    }
}
