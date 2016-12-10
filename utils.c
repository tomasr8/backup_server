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
