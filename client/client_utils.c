#include "client_utils.h"

int parse_send_recv(socklen_t sock, char *buffer) {

    request req = { UNKNOWN, 0, 0, {0} };

    if(!parse_line(buffer, &req)) {
        return E_PARSE;
    }

    fprintf(stderr, "to send> cmd:%d resource:%d (%d bytes)<%s>\n", req.cmd, req.res, req.len, req.data);

    if(!send_request(sock, &req)) {
        return E_CONNECTION;
    }

    fprintf(stderr, "request sent\n");

    response res = { UNKNOWN, 0, 0, {0} };

    if(!receive_response(sock, &res)) {
        fprintf(stderr, "Error reading response.. \n");
        return E_CONNECTION;
    }

    printf("Response> %d lm:%d (%d)<%s>\n", res.status, res.lm, res.len, res.data);

    return 0;
}

bool parse_line(char *buffer, request *req) {
    char cmdStr[4] = {0};
    char data[257] = {0};
    int resource = -1;

    int numRead = sscanf(buffer, "%3s%*[ ]%d%256[^\n]s", cmdStr, &resource, data);

    if(resource < 0 || resource > RESOURCE_MAX) {
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
    req->len = (uint16_t) (strlen(data) > 0 ? strlen(data) - 1 : 0); // not counting the leading space
    strncpy(req->data, data + 1, 256); // data[0] is space separating resource and data

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
