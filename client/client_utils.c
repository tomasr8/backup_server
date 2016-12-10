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

    response res = { UNKNOWN, 0, {0} };

    if(!receive_response(sock, &res)) {
        fprintf(stderr, "Error reading response.. \n");
        return E_CONNECTION;
    }

    printf("Response> %d (%d)<%s>\n", res.status, res.len, res.data);

    return 0;
}

int get_socket_multiple(char **IPs, int *ports, size_t len, struct sockaddr_in *addr, int id) {
    int sock = -1;
    for(int i = 0; i < len; i++) {
        if((sock = get_socket(IPs[i], ports[i], addr, id)) >= 0) {
            break;
        }
    }
    return sock;
}

int get_socket(char const *ip, int port, struct sockaddr_in *addr, int id) {
    int sock;
    fd_set fdset;
    struct timeval tv;

    fprintf(stderr, "trying to connect to %s on port %d\n", ip, port);

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ) {
        fprintf(stderr, "socket() call failed\n");
        return -1;
    }

    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(ip);
    addr->sin_port = htons(port);

    if(fcntl(sock, F_SETFL, O_NONBLOCK) < 0) {
        close(sock);
        fprintf(stderr, "Could not set socket to non-blocking\n");
        return -1;
    }

    connect(sock, (struct sockaddr *) addr, sizeof(*addr));

    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    tv.tv_sec = 10;             /* 10 second timeout */
    tv.tv_usec = 0;

    if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1) {
        int so_error;
        socklen_t len = sizeof so_error;

        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0) {
            int opts;

            if((opts = fcntl(sock, F_GETFL)) < 0) {
                fprintf(stderr, "fnctl(F_GETFL) failed\n");
                close(sock);
                return -1;
            }

            opts = opts & (~O_NONBLOCK);

            if (fcntl(sock, F_SETFL, opts) < 0) {
                fprintf(stderr, "fnctl(F_SETFL) failed\n");
                close(sock);
                return -1;
            }

            if(!send_id(sock, id)) {
                fprintf(stderr, "Failed to send identification\n");
                return -1;
            }

            fprintf(stderr, "Socket is open\n");
            return sock;
        }

        fprintf(stderr, "Connection failed with error %d\n", so_error);
    }

    close(sock);

    return -1;
}

bool receive_response(int socket, response *res) {

    if(!read_uint16(socket, &res->status)) {
        return false;
    }

    if(!read_uint16(socket, &res->len)) {
        return false;
    }

    if(res->len == 0) {
        res->data[0] = '\0';
        return true;
    }

    if(!read_str(socket, res->data, res->len)) {
        return false;
    }

    return true;
}

bool send_request(int sock, request *req) {
    bool res = send_uint16(sock, req->cmd) &&
        send_uint16(sock, req->res) &&
        send_uint16(sock, req->len);

    if(!res) {
        fprintf(stderr, "Failed to send request parameters\n");
        return false;
    } else if(req->len == 0) {
        return true;
    }

    if(send(sock, req->data, req->len, 0) <= 0) {
        fprintf(stderr, "Failed to send data\n");
        return false;
    }

    return true;


    //uint16_t num;
    // num = htons(req->cmd);
    // if(send(sock, &num, sizeof(uint16_t), 0) <= 0) {
    //     fprintf(stderr, "Failed to send cmd\n");
    //     return false;
    // }
    //
    // num = htons(req->res);
    // if(send(sock, &num, sizeof(uint16_t), 0) <= 0) {
    //     fprintf(stderr, "Failed to send resource\n");
    //     return false;
    // }
    //
    // num = htons(req->len);
    // if(send(sock, &num, sizeof(uint16_t), 0) <= 0) {
    //     fprintf(stderr, "Failed to send data length\n");
    //     return false;
    // }
    //
    // if(req->len == 0) {
    //     return true;
    // }
    //
    // if(send(sock, req->data, req->len, 0) <= 0) {
    //     fprintf(stderr, "Failed to send data\n");
    //     return false;
    // }
    //
    // return true;
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
    req->len = (uint16_t) (strlen(data) > 0 ? strlen(data) - 1 : 0); // not counting the leading space
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
