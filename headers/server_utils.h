#include "utils.h"

#ifndef SERVER_UTILS_H_
#define SERVER_UTILS_H_

/**
* reads a single request into a struct
* @returns true on success
*/
bool receive_request(int socket, request *req);

/**
* sends a response struct to a socket
* @returns true on success
*/
bool send_response(int socket, response *res);

#endif
