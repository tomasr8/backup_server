#include <signal.h>
#include "utils.h"

#ifndef CLIENT_UTILS_H_
#define CLIENT_UTILS_H_


#define LINE_SIZE 264


/**
* parses single line from stdin into request struct
*
* @param line - null-terminated string
* @param req - request struct
*
* @return true on success, false on failure
*/
bool parse_line(char *line, request *req);

/**
* converts textual command ("get", "set") to its integer equivalent
* defined in constants.h
*
* @param cmd - command
*
* @return integer representation of the given command or UNKNOWN
*/
int to_number(char *cmd);

/**
* parses <line>, sends request and reads response
*
* @param sock - socket
* @param line - one line from stdin
*
* @return 0 on success, E_PARSE for parsing error, E_CONNECTION for socket error
*/
int parse_send_recv(socklen_t sock, char *line);

#endif
