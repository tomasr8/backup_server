#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "utils.h"
#include "server_utils.h"

bool check_resources(char *dir) {
    char* path_to_file;
    FILE *fp;
    for(int i = 0; i <= RESOURCE_MAX; i++) {
        if((path_to_file = path_join(dir, i)) == NULL) {
            fprintf(stderr, "error ocurred in path_join()\n");
            return false;
        }
        fprintf(stderr, "path to file: %s\n", path_to_file);

        if((fp = fopen(path_to_file, "ab+")) == NULL) {
            fprintf(stderr, "could not open file\n");
            return false;
        }

        free(path_to_file);
        fclose(fp);
    }

    return true;
}

char *path_join(char *dir, int file_no) {
    const int path_len = strlen(dir) + 8 + (int)floor(log(RESOURCE_MAX)/log(10));
    char * joined;

    if((joined = malloc(path_len)) == NULL) {
        fprintf(stderr, "path_join(): failed malloc() enough memory\n");
        return NULL;
    }

    char template[] = "%sres%d.txt";
    char template_slash[] = "%s/res%d.txt";

    if(dir[strlen(dir) - 1] == '/') {
        sprintf(joined, template, dir, file_no);
    } else {
        sprintf(joined, template_slash, dir, file_no);
    }

    return joined;
}

bool send_response(int socket, response *res) {
    uint16_t num;

    num = htons(res->status);
    if(send(socket, &num, sizeof(uint16_t), 0) <= 0) {
        printf("Failed to send cmd\n");
        return false;
    }

    num = htons(res->len);
    if(send(socket, &num, sizeof(uint16_t), 0) <= 0) {
        printf("Failed to send resource\n");
        return false;
    }

    if(res->len == 0) {
        return true;
    }

    if(send(socket, res->data, res->len, 0) <= 0) {
        printf("Failed to send data\n");
        return false;
    }

    return true;
}

bool read_request(int socket, request *req) {

    if(!read_uint16(socket, &req->cmd)) {
        return false;
        //dieWithError("Error reading command\n");
    }

    if(!read_uint16(socket, &req->res)) {
        return false;
        //dieWithError("Error reading data length\n");
    }

    if(!read_uint16(socket, &req->len)) {
        return false;
        //dieWithError("Error reading data length\n");
    }

    if(req->len == 0) {
        //printf("Received response from server: Status code: %d\n", res.status);
        req->data[0] = '\0';
        return true;
    }

    if(!read_str(socket, req->data, req->len)) {
        return false;
        //dieWithError("Error reading data\n");
    }

    return true;
}
