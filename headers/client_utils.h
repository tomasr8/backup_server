#include "utils.h"

#ifndef CLIENT_UTILS_H_
#define CLIENT_UTILS_H_

/**
* reads a single response into a struct
* @returns true on success
*
*/
bool receive_response(int socket, response *res);

/**
* serialiyes and sends a request to server
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

#endif
