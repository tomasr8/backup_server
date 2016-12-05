#include <stdbool.h>
#include <stdint.h>

#ifndef UTILS_H_
#define UTILS_H_

typedef struct request {
    uint8_t cmd;
    uint8_t res;
    uint8_t len;
    char data[255];
} request;

typedef struct response {
    uint8_t status;
    uint8_t len;
    char data[256];
} response;

bool read_uint8(int socket, uint8_t *num);
//bool send_uint16(int socket, uint16_t num);
bool readString(int socket, char *buffer, int len);

#endif
