#include "server_utils.h"

bool check_resources(char *dir) {
    char* path_to_file;
    FILE *fp;
    for(int i = 0; i <= RESOURCE_MAX; i++) {
        if((path_to_file = path_join(dir, i)) == NULL) {
            log_err("error ocurred in path_join()\n");
            return false;
        }
        log_debug("path to file: %s\n", path_to_file);

        if((fp = fopen(path_to_file, "ab+")) == NULL) {
            log_err("Could not open file: %s\n", path_to_file);
            return false;
        }

        free(path_to_file);
        fclose(fp);
    }

    return true;
}

char *path_join(char *dir, int res) {
    const int path_len = strlen(dir) + 8 + (int)floor(log(RESOURCE_MAX)/log(10));
    char * joined;

    if((joined = malloc(path_len)) == NULL) {
        log_err("path_join(): failed to malloc() enough memory\n");
        return NULL;
    }

    char template[] = "%sres%d.txt";
    char template_slash[] = "%s/res%d.txt";

    if(dir[strlen(dir) - 1] == '/') {
        sprintf(joined, template, dir, res);
    } else {
        sprintf(joined, template_slash, dir, res);
    }

    return joined;
}

bool send_response(int socket, response *res) {

    if(!send_uint16(socket, res->status)) {
        printf("Failed to send res->status\n");
        return false;
    }


    if(!send_uint32(socket, res->lm)) {
        printf("Failed to send res->lm\n");
        return false;
    }

    if(!send_uint16(socket, res->len)) {
        printf("Failed to send res->len\n");
        return false;
    }

    if(res->len == 0) {
        return true;
    }

    if(send(socket, res->data, res->len, 0) <= 0) {
        printf("Failed to send res->data\n");
        return false;
    }

    return true;
}

bool read_request(int socket, request *req) {

    if(!read_uint16(socket, &req->cmd)) {
        return false;
    }

    if(!read_uint16(socket, &req->res)) {
        return false;
    }

    if(!read_uint16(socket, &req->len)) {
        return false;
    }

    if(req->len == 0) {
        req->data[0] = '\0';
        return true;
    }

    if(!read_str(socket, req->data, req->len)) {
        return false;
    }

    return true;
}

bool set_resource(char *path, int res, char *data, pthread_mutex_t *mutex) {
    char *fullpath = path_join(path, res);

    if(fullpath == NULL) {
        log_err("Failed to join path\n");
        return false;
    }

    log_debug("locking mutex before writing to file\n");
    pthread_mutex_lock(mutex);

    if(!write_to_file(fullpath, data)) {
        log_warn("Failed to write to file\n");
    }

    pthread_mutex_unlock(mutex);
    free(fullpath);

    return true;
}

bool write_to_file(char *path, char *data) {
    FILE *fp;

    if((fp = fopen(path, "w")) == NULL) {
        log_warn("Could not open file for writing\n");
        return false;
    }

    fprintf(fp, "%s", data);
    fclose(fp);

    return true;
}

void fill_response(response *res, int status, char *data, uint32_t lm) {
    res->status = status;
    res->lm = lm;

    const size_t len = strlen(data);

    strncpy(res->data, data, len);
    res->data[len] = '\0';
    res->len = len;
}
