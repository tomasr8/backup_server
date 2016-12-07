#include "utils.h"

#ifndef SERVER_UTILS_H_
#define SERVER_UTILS_H_

bool receive_request(int socket, request *req);
bool send_response(int socket, response *res);

#endif
