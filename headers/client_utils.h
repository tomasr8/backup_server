#include "utils.h"

#ifndef CLIENT_UTILS_H_
#define CLIENT_UTILS_H_

bool receive_response(int socket, response *res);
bool send_request(int socket, request *req);
bool parseRequest(char *buffer, request *req);
int toNumber(char *command);

#endif
