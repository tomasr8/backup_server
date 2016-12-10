#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include "commands.h"
#include "utils.h"

#ifndef CLIENT_UTILS_H_
#define CLIENT_UTILS_H_

#define LINE_SIZE 264
/**
* reads a single response into a struct
* @returns true on success
*
*/
bool receive_response(int socket, response *res);

/**
* serializes and sends a request to a server
* @returns true on success
*
*/
bool send_request(int socket, request *req);

/**
* parses a single line in <buffer> into struct <req>
* @returns true on success
*
*/
bool parse_line(char *buffer, request *req);

/**
* converts textual command (e.g. "get", "set") to its integer equivalent
* defined in commands.h
*
*/
int to_number(char *command);

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

/**
* reads commands from stdin, parses them, sends them,
* receives response and prints it to stdout.
* handles closing socket <sock>
* @returns true if EOF was reached, on error returns false.
*/
int parse_send_recv(socklen_t sock, char *buffer);

#endif
