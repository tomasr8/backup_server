#include <math.h>
#include <pthread.h>
#include <signal.h>
#include "utils.h"

#ifndef SERVER_UTILS_H_
#define SERVER_UTILS_H_

/*
* utility functions for the server part of the application
*
*/

/**
* reads a single request into a struct
*
* @param socket
* @param req - struct into which the function reads data
* @return true on success, false on failure
*/
bool read_request(int socket, request *req);

/**
* sends a response struct over a socket
*
* @param socket
* @param res - response struct to send
* @return true on success, false on failure
*/
bool send_response(int socket, response *res);

/**
* returns full path from directory and resource number
* example: ./data/, 2 -> ./data/res2.txt
* caller responsible for freeing memory
*
* @param dir - path to directory which contains
* @param resrc - resource number
* @return full path to resource
*/
char *path_join(char *dir, int resrc);

/**
* Checks that all resources are available, creates them if not
*
* @param dir - path to directory containing resources
*
* @return true on success, false on failure
*/
bool check_resources(char *dir);

/**
* writes null-terminated string to file
*
* @param path - full path to file
* @param data - null terminated data
*
* @return true on success, false on failure
*/
bool write_to_file(char *path, char *data);

/**
* wrapper for write_to_file().
* Uses mutex to ensure data consistency,
* if multiple threads were trying to write at the same time
*
* @param path - path to resource directory
* @param resrc - resource number
* @param data - null-terminated string
* @param mutex
*
* @return true on success, false on failure
*/
bool set_resource(char *path, int resrc, char *data, pthread_mutex_t *mutex);

/**
* handles single client request, including sending response
*
* @param client_sock - client socket
* @param server_sock - socket to second server, can be -1
* @param req - request struct
* @param mutex_arr - arraz of mutexes
*
* @return true on success, false on failure
* #TODO haven't decided what should actually be treated as a failure
* handle_connection() closes socket if this function returns false
*/
bool handle_client_request(int client_sock, int server_sock, request *req, pthread_mutex_t *mutex_arr);

/**
* handles single server request, including sending response
*
* @param server_sock - socket to second server, can be -1
* @param req - request struct
* @param mutex_arr - arraz of mutexes
*
* @return true on success, false on failure
* #TODO haven't decided what should actually be treated as a failure
* handle_connection() closes socket if this function returns false
*/
bool handle_server_request(int server_sock, request *req, pthread_mutex_t *mutex_arr);

/**
* reads requests and dispatches them to handle_client_request() or
* handle_server_request()
* This function is run in a thread for each new connection
*
* @param sock_ptr - pointer to socket
*/
void * handle_connection(void * sock_ptr);

/**
* simple wrapper that properties to a response
*
* @param res - pointer to response
* @param status - response status
* @param data - pointer to null-terminated data
* @param lm - last modified date as a unix timestamp
*/
void fill_response(response *res, int status, char *data, uint32_t lm);

#endif
