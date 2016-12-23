#include "utils.h"

bool last_modified(char *path, uint32_t *lm) {
    struct stat buf;

    const int ret = stat(path, &buf);

    if(ret != 0) {
        const int err = errno;
        if(err == EACCES) {
            fprintf(stderr, "Search permission denied in stat()\n");
        } else {
            fprintf(stderr, "Error occurred in stat() call\n");
        }
        return false;
    }

    *lm = (uint32_t) difftime(buf.st_ctime, 0);
    //fprintf(stderr, "%d\n", *lm);
    return true;
}

bool read_file(char *path, char *buffer) {
    FILE *f = fopen(path, "rb");

    if(f == NULL) {
        fprintf(stderr, "Error opening file %s\n", path);
        return false;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if(fsize > MAX_SIZE) {
        fprintf(stderr, "File is larger than MAX_SIZE\n");
        fclose(f);
        return false;
    }

    fread(buffer, fsize, 1, f);
    fclose(f);
    buffer[MAX_SIZE] = '\0'; // buffer needs to be of size at least MAX_SIZE+1
    return true;
}

void dieWithError(const char *errorMessage) {
    perror (errorMessage);
    exit(1);
}

bool send_id(int sock, uint16_t id) {
    id = htons(id);
    return send(sock, &id, sizeof(uint16_t), 0) > 0;
}

bool send_uint16(int sock, uint16_t num) {
    num = htons(num);
    return send(sock, &num, sizeof(uint16_t), 0) > 0;
}

bool send_uint32(int sock, uint32_t num) {
    num = htonl(num);
    return send(sock, &num, sizeof(uint32_t), 0) > 0;
}

bool read_uint32(int sock, uint32_t *num) {
    uint32_t ret;
    char *data = (char*)&ret;
    int left = sizeof(ret);
    int rc;

    do {
        rc = recv(sock, data, left, 0);
        if (rc <= 0) {
            return false;
        } else {
            data += rc;
            left -= rc;
        }
    } while (left > 0);

    *num = ntohl(ret);
    return true;
}

bool read_uint16(int sock, uint16_t *num) {
    uint16_t ret;
    char *data = (char*)&ret;
    int left = sizeof(ret);
    int rc;

    do {
        rc = recv(sock, data, left, 0);
        if (rc <= 0) {
            return false;
        } else {
            data += rc;
            left -= rc;
        }
    } while (left > 0);

    *num = ntohs(ret);
    return true;
}

bool read_str(int sock, char *buffer, int len) {
    int left = len;
    char *data = buffer;
    int rc;

    do {
        rc = recv(sock, data, left, 0);
        if (rc <= 0) {
            buffer[0] = '\0';
            return false;
        } else {
            data += rc;
            left -= rc;
        }
    } while (left > 0);

    buffer[len] = '\0';
    return true;
}

bool send_request(int sock, request *req) {
    bool res = send_uint16(sock, req->cmd) &&
        send_uint16(sock, req->res) &&
        send_uint16(sock, req->len);

    if(!res) {
        fprintf(stderr, "Failed to send request parameters\n");
        return false;
    } else if(req->len == 0) {
        return true;
    }

    if(send(sock, req->data, req->len, 0) <= 0) {
        fprintf(stderr, "Failed to send data\n");
        return false;
    }

    return true;
}

bool receive_response(int socket, response *res) {

    if(!read_uint16(socket, &res->status)) {
        return false;
    }

    if(!read_uint32(socket, &res->lm)) {
        return false;
    }

    if(!read_uint16(socket, &res->len)) {
        return false;
    }

    if(res->len == 0) {
        res->data[0] = '\0';
        return true;
    }

    if(!read_str(socket, res->data, res->len)) {
        return false;
    }

    return true;
}

int get_socket_multiple(char **IPs, int *ports, size_t len, struct sockaddr_in *addr, int id) {
    int sock = -1;
    for(int i = 0; i < len; i++) {
        if((sock = get_socket(IPs[i], ports[i], addr, id)) >= 0) {
            break;
        }
    }
    return sock;
}

int get_socket(char const *ip, int port, struct sockaddr_in *addr, int id) {
    int sock;
    fd_set fdset;
    struct timeval tv;

    fprintf(stderr, "trying to connect to %s on port %d\n", ip, port);

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ) {
        fprintf(stderr, "socket() call failed\n");
        return -1;
    }

    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr(ip);
    addr->sin_port = htons(port);

    if(fcntl(sock, F_SETFL, O_NONBLOCK) < 0) {
        close(sock);
        fprintf(stderr, "Could not set socket to non-blocking\n");
        return -1;
    }

    connect(sock, (struct sockaddr *) addr, sizeof(*addr));

    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    tv.tv_sec = 10;             /* 10 second timeout */
    tv.tv_usec = 0;

    if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1) {
        int so_error;
        socklen_t len = sizeof so_error;

        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0) {
            int opts;

            if((opts = fcntl(sock, F_GETFL)) < 0) {
                fprintf(stderr, "fnctl(F_GETFL) failed\n");
                close(sock);
                return -1;
            }

            opts = opts & (~O_NONBLOCK);

            if (fcntl(sock, F_SETFL, opts) < 0) {
                fprintf(stderr, "fnctl(F_SETFL) failed\n");
                close(sock);
                return -1;
            }

            if(!send_id(sock, id)) {
                fprintf(stderr, "Failed to send identification\n");
                return -1;
            }

            fprintf(stderr, "Socket is open\n");
            return sock;
        }

        fprintf(stderr, "Connection failed with error %d\n", so_error);
    }

    close(sock);

    return -1;
}
