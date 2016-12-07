#include <stdbool.h>
#include <stdint.h>

#ifndef UTILS_H_
#define UTILS_H_

typedef struct request {
    uint16_t cmd;
    uint16_t res;
    uint16_t len;
    char data[256];
} request;

typedef struct response {
    uint16_t status;
    uint16_t len;
    char data[256];
} response;

bool read_uint16(int socket, uint16_t *num);
//bool send_uint16(int socket, uint16_t num);
bool read_str(int socket, char *buffer, int len);

#endif
