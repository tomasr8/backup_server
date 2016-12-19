#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "utils.h"

void dieWithError(const char *errorMessage) {
    perror (errorMessage);
    exit(1);
}

bool send_id(int sock, uint16_t id) {
    id = htons(id);
    return send(sock, &id, sizeof(uint16_t), 0) > 0;
}

bool send_uint16(int sock, uint16_t num) {
    num = htons(num);
    return send(sock, &num, sizeof(uint16_t), 0) > 0;
}

bool read_uint16(int sock, uint16_t *num) {
    uint16_t ret;
    char *data = (char*)&ret;
    int left = sizeof(ret);
    int rc;

    do {
        rc = recv(sock, data, left, 0);
        if (rc <= 0) {
            return false;
        } else {
            data += rc;
            left -= rc;
        }
    } while (left > 0);

    *num = ntohs(ret);
    return true;
}

bool read_str(int sock, char *buffer, int len) {
    int left = len;
    char *data = buffer;
    int rc;

    do {
        rc = recv(sock, data, left, 0);
        if (rc <= 0) {
            buffer[0] = '\0';
            return false;
        } else {
            data += rc;
            left -= rc;
        }
    } while (left > 0);

    buffer[len] = '\0';
    return true;
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
