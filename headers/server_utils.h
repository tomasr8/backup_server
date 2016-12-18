#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <math.h>
#include <pthread.h>
#include "utils.h"

#ifndef SERVER_UTILS_H_
#define SERVER_UTILS_H_

/**
* reads a single request into a struct
* @returns true on success
*/
bool read_request(int socket, request *req);

/**
* sends a response struct to a socket
* @returns true on success
*/
bool send_response(int socket, response *res);

char *path_join(char *dir, int file_no);

bool check_resources(char *dir);

bool set_resource(char *path, int res, char *data, pthread_mutex_t *mutex);
bool handle_client_request(int sock, request *req, pthread_mutex_t *mutex_arr);
bool handle_server_request(int sock, request *req, pthread_mutex_t *mutex_arr);
bool write_to_file(char *path, char *data);

void fill_response(response *res, int status, char *data);

#endif
