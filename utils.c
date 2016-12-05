#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "utils.h"

bool read_uint8(int socket, uint8_t *num) {
    uint8_t ret;
    char *data = (char*)&ret;
    int left = sizeof(ret);
    int rc;

    do {
        rc = recv(socket, data, left, 0);
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

bool readString(int socket, char *buffer, int len) {
    int left = len;
    char *data = buffer;
    int rc;

    do {
        rc = recv(socket, data, left, 0);
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
