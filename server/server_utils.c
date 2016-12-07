#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "err.h"
#include "commands.h"
#include "utils.h"
#include "server_utils.h"


bool send_response(int socket, response *res) {
    uint16_t num;

    num = htons(res->status);
    if(send(socket, &num, sizeof(uint16_t), 0) <= 0) {
        printf("Failed to send cmd\n");
        return false;
    }

    num = htons(res->len);
    if(send(socket, &num, sizeof(uint16_t), 0) <= 0) {
        printf("Failed to send resource\n");
        return false;
    }

    if(res->len == 0) {
        return true;
    }

    if(send(socket, res->data, res->len, 0) <= 0) {
        printf("Failed to send data\n");
        return false;
    }

    return true;
}

bool receive_request(int socket, request *req) {

    if(!read_uint16(socket, &req->cmd)) {
        return false;
        //dieWithError("Error reading command\n");
    }

    if(!read_uint16(socket, &req->res)) {
        return false;
        //dieWithError("Error reading data length\n");
    }

    if(!read_uint16(socket, &req->len)) {
        return false;
        //dieWithError("Error reading data length\n");
    }

    if(req->len == 0) {
        //printf("Received response from server: Status code: %d\n", res.status);
        req->data[0] = '\0';
        return true;
    }

    if(!read_str(socket, req->data, req->len)) {
        return false;
        //dieWithError("Error reading data\n");
    }

    return true;
}
