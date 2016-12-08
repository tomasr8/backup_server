#include <stdbool.h>
#include <stdint.h>

#ifndef UTILS_H_
#define UTILS_H_

/**
* struct used to represent a single request
* .cmd = command e.g. GET, SET, ...
* .res = resource, 0-32, theoretically could be more
* .len = length of data being sent in bytes
* .data = request data
*
*/
typedef struct request {
    uint16_t cmd;
    uint16_t res;
    uint16_t len;
    char data[256];
} request;

/**
* struct represting a single response
* .status = response status, e.g. OK, ERROR, ...
* .len = length of message being sent
* .data = reponse data
*
*/
typedef struct response {
    uint16_t status;
    uint16_t len;
    char data[256];
} response;

/**
* shortuct for perror() & exit(1)
*/
void dieWithError(char const *errorMessage);

/**
* reads 4 bytes from a socet into a usigned 16 bit integer
* @returns true on success
*/
bool read_uint16(int socket, uint16_t *num);

/**
* reads <len> bytes from socket into <buffer>,
* appends null terminator to the end
* @returns true on success
*/
bool read_str(int socket, char *buffer, int len);

#endif
