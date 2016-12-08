#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
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

socklen_t get_socket(char const *ip, int port, struct sockaddr_in *addr);

void read_send_recv(socklen_t sock);

#endif
