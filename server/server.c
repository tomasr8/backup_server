#include "server_utils.h"

#define MAXPENDING 5    /* Maximum outstanding connection requests */
pthread_mutex_t mutex_arr[RESOURCE_MAX];
char *dir;

void * handle_connection(void * sock_ptr);   /* TCP client handling function */
bool handle_client(int sock);


int main(int argc, char *argv[]) {

    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */

    if (argc != 3) {
        fprintf(stderr, "Usage: <Server Port> <folder>\n");
        exit(1);
    }

    dir = argv[2];
    echoServPort = atoi(argv[1]);

    if(!check_resources(argv[2])) {
        fprintf(stderr, "failed to access required resources\n");
        return 1;
    }

    for(int i = 0; i < RESOURCE_MAX; i++) {
        if(pthread_mutex_init(&mutex_arr[i], NULL) != 0) {
            fprintf(stderr, "failed to create mutex no: %d\n", i);
            return 1;
        }
    }

    // for(int i = 0; i < RESOURCE_MAX; i++) {
    //     pthread_mutex_destroy(&mutex_arr[i]);
    // }

    /* Create socket for incoming connections */
    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        dieWithError("socket() failed");
    }

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
        dieWithError("bind() failed");
    }

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0) {
        dieWithError("listen() failed");
    }

        /* Set the size of the in-out parameter */
    clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
    while ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) >= 0) {
        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
        pthread_t worker;
        int sock = clntSock;
        // int *sock_ptr = malloc(sizeof(int));
        // *sock_ptr = clntSock;

        if(pthread_create(&worker, NULL, handle_connection, (void *)&sock)) {
            fprintf(stderr, "Failed to create a thread\n");
            close(sock);
            //free(sock_ptr);
        }
    }

}

bool handle_client_request(int sock, request *req, pthread_mutex_t *mutex_arr) {
    response res = { UNKNOWN, 0, {0} };

    printf("Client request> %d %d (%d)<%s>\n", req->cmd, req->res, req->len, req->data);

    if(req->res < 0 || req->res > RESOURCE_MAX) {
        fprintf(stderr, "invalid resource: %d\n", req->res);
        return true;
    }

    if(req->cmd == SET) {
        if(!set_resource(dir, req->res, req->data, &mutex_arr[req->res])) {
            fprintf(stderr, "Failed to set resource\n");

            fill_response(&res, ERROR, "Failed to set resource");
        } else {
            fill_response(&res, OK, "Success");
        }
    } else {
        fill_response(&res, ERROR, "Not implemented");
    }


    // if(req->cmd == GET) {
    //     char *path = path_join(dir, req->res);
    //     FILE *fp = fopen(path, "r");
    //
    //     if(fp == NULL) {
    //         fprintf(stderr, "Error opening file\n");
    //         res.status = ERROR;
    //         res.data = "Error opening file";
    //         res.len = strlen(res.data);
    //     } else {
    //         char buffer[MAX_SIZE + 1];
    //         size_t len = fread(buffer, sizeof(char), MAX_SIZE, fp);
    //         if ( ferror( fp ) != 0 ) {
    //             fprintf(stderr, "Error reading file\n");
    //             res.status = ERROR;
    //             res.data = "Error reading file";
    //             res.len = strlen(res.data);
    //         } else {
    //             fprintf(stderr, "file read successfull\n");
    //             buffer[len] = '\0'; /* Just to be safe. */
    //             res.status = OK;
    //             res.data = buffer;
    //             res.len = strlen(res.data);
    //         }
    //
    //         fclose(fp);
    //     }
    // } else {
    //     res.status = OK;
    //     res.data = "Success";
    //     res.len = strlen(res.data);
    // }

    if(!send_response(sock, &res)) {
        fprintf(stderr, "Error sending response\n");
        return false;
    }

    fprintf(stderr, "response sent\n");

    return true;
}

void *handle_connection(void *sock_ptr) {
    int sock = *((int *)sock_ptr);

    fprintf(stderr, "thread started\n");

    uint16_t id;
    if(!read_uint16(sock, &id)) {
        fprintf(stderr, "Connection failed to send identification\n");
        return 0;
    }

    fprintf(stderr, "id: %d\n", (int)id);

    request req = { UNKNOWN, 0, 0, {0} };
    while(read_request(sock, &req)) {
        if(id == CLIENT) {
            fprintf(stderr, "handling client request\n");
            if(!handle_client_request(sock, &req, mutex_arr)) break;
        } else {
            fprintf(stderr, "server handler not implemented\n");
            break;
        }
    }

    close(sock);    /* Close client socket */
    //free(sock_ptr);
    fprintf(stderr, "thread finished\n");
    return 0;
}
