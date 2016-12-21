#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include "constants.h"

#ifndef UTILS_H_
#define UTILS_H_

#define MAX_SIZE 255

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
    char data[MAX_SIZE + 1]; // null byte, so we can treat it as string
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
    uint32_t lm;
    uint16_t len;
    char data[MAX_SIZE + 1];
} response;

/**
* shortuct for perror() & exit(1)
*/
void dieWithError(char const *errorMessage);

/**
* reads 4 bytes from a socet into a usigned 16 bit integer
* @returns true on success
*/
bool read_uint16(int sock, uint16_t *num);

/**
* reads <len> bytes from socket into <buffer>,
* appends null terminator to the end
* @returns true on success
*/
bool read_str(int sock, char *buffer, int len);

/**
* serializes and sends a request to a server
* @returns true on success
*
*/
bool send_request(int sock, request *req);

/**
* reads a single response into a struct
* @returns true on success
*
*/
bool receive_response(int sock, response *res);

bool send_id(int sock, uint16_t id);

bool send_uint16(int sock, uint16_t num);

bool send_uint32(int sock, uint32_t num);

bool read_uint32(int sock, uint32_t *num);

/**
* @returns socket for <ip> and <port> and fills struct <addr>
* on error returns -1;
*/
int get_socket(char const *ip, int port, struct sockaddr_in *addr, int id);

/**
* returns socket for one of the <IPs> and <ports>
* or -1 if no connection can be made
*/
int get_socket_multiple(char **IPs, int *ports, size_t len, struct sockaddr_in *addr, int id);

#endif
