#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <stdarg.h>
#include "constants.h"


#ifndef UTILS_H_
#define UTILS_H_

/*
* various io and socket utility functions and type definitions
*/

// max resource size
#define MAX_SIZE 255

/**
* struct used to represent a single request
* .cmd = command e.g. GET, SET, ...
* .res = resource, 0-32, theoretically could be more if RESOURCE_MAX in constants.h is changed
* .len = length of data being sent in bytes
* .data = request data
*
*/
typedef struct request {
    uint16_t cmd;
    uint16_t res;
    uint16_t len;
    char data[MAX_SIZE + 1]; // null byte, so we can treat it as a string
} request;

/**
* struct representing a single response
* .status = response status, e.g. OK, ERROR, ...
* .len = length of message being sent
* .data = response data
*
*/
typedef struct response {
    uint16_t status;
    uint32_t lm;
    uint16_t len;
    char data[MAX_SIZE + 1];
} response;

/**
* sends unsigned 16-bit integer over a socket
*
* @param sock - socket
* @param num - number to send_uint16
*
* @return true on success, false on failure
*/
bool send_uint16(int sock, uint16_t num);

/**
* 32-bit equivalent of send_uint16(sock, num)
*/
bool send_uint32(int sock, uint32_t num);

/**
* syntactic sugar for send_uint16(sock, id);
*/
bool send_id(int sock, uint16_t id);

/**
* reads 2 bytes from a socket into a usigned 16 bit integer
*
* @param sock - socket
* @param num - pointer to uint16_t where the number will be stored
*
* @return true on success, false on failure
*/
bool read_uint16(int sock, uint16_t *num);

/**
* 32-bit equivalent of read_uint16(sock, num)
*/
bool read_uint32(int sock, uint32_t *num);

/**
* reads <len> bytes from socket into <buffer>,
* appends null terminator to the end
*
* @param sock - socket
* @param buffer - buffer into which the data will be read, the buffer length has to be atleast len+1
* @param len - how many bytes to read, function appends null at the end
* @return true on success, false on failure
*/
bool read_str(int sock, char *buffer, int len);

/**
* sends a request struct over a socket
*
* @param sock - socket
* @param req - request struct
*
* @return true on success, false on failure
*/
bool send_request(int sock, request *req);

/**
* reads a single response struct from a socket
*
* @param sock - socket
* @param res - response struct into which the data is read
*
* @return true on success, false on failure
*/
bool receive_response(int sock, response *res);

/**
* attempts to establish a connection with specified ip on given port
*
* @param ip - server ip
* @param port - server port
* @param addr - connections settings, will be available after connection
* @param id - identification, get_socket sends identification before it returns
*
* @return socket number on success, -1 on error
*/
int get_socket(char const *ip, int port, struct sockaddr_in *addr, int id);

/**
* similar to get_socket(), tries to connect to multiple hosts
* returns as soon as a connection has been established
*
* @param IPs - pointer to ip addresses
* @param ports - pointer to ports
* @param len - number of hosts to trz to connect to
* @param addr - connection options
* @param id - identification
*
* @return socket on success, -1 on failure
* failure = no hosts were available
*/
int get_socket_multiple(char **IPs, int *ports, int len, struct sockaddr_in *addr, int id);

/**
* reads date of last modification of a file into <lm>
*
* @param path - full path to file
* @param lm - the date will be available in this variable
*
* @return true on success, false on failure
*/
bool last_modified(char *path, uint32_t *lm);

/**
* reads the whole file into <buffer> if it does not exceed MAX_SIZE
* appends null byte at the end
*
* @param path - full path to file
* @param buffer
*
* @return true on success, false on failure
*/
bool read_file(char *path, char *buffer);

/**
* wrappers for syslog and fprintf(stderr, ...)
*/
void log_err(char *format, ...);
void log_info(char *format, ...);
void log_notice(char *format, ...);
void log_warn(char *format, ...);
void log_debug(char *format, ...);

/**
* for some reason stdarg.h does not seem declare vsyslog..???
*
*/
void vsyslog(int priority, const char *format, va_list ap);

#endif
