#include "server_utils.h"

#define MAXPENDING 5    /* Maximum outstanding connection requests */
pthread_mutex_t mutex_arr[RESOURCE_MAX + 1]; // 0-32 = 33
char *dir; // path to resources
char *second_server_ip;
int second_server_port;

void * handle_connection(void * sock_ptr);   /* TCP client handling function */
bool handle_client(int sock);


int main(int argc, char *argv[]) {

    int server_sock;                    /* Socket descriptor for server */
    int client_sock;                    /* Socket descriptor for client */
    struct sockaddr_in server_addr; /* Local address */
    struct sockaddr_in client_addr; /* Client address */
    unsigned short server_port;     /* Server port */
    unsigned int client_addr_size = sizeof(client_addr);/* Length of client address data structure */

    if (argc != 5) {
        fprintf(stderr, "Usage: <Server Port> <Second server port> <Second server address> <folder>\n");
        exit(1);
    }

    second_server_port = atoi(argv[2]);
    second_server_ip = argv[3];
    dir = argv[4];
    server_port = atoi(argv[1]);

    if(!check_resources(argv[4])) {
        fprintf(stderr, "failed to access required resources\n");
        return EXIT_FAILURE;
    }

    for(int i = 0; i < RESOURCE_MAX + 1; i++) {
        if(pthread_mutex_init(&mutex_arr[i], NULL) != 0) {
            fprintf(stderr, "failed to create mutex no: %d\n", i);
            return EXIT_FAILURE;
        }
    }

    signal(SIGPIPE, SIG_IGN);

    // for(int i = 0; i < RESOURCE_MAX; i++) {
    //     pthread_mutex_destroy(&mutex_arr[i]);
    // }

    /* Create socket for incoming connections */
    if ((server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        dieWithError("socket() failed");
    }

    /* Construct local address structure */
    memset(&server_addr, 0, sizeof(server_addr));   /* Zero out structure */
    server_addr.sin_family = AF_INET;                /* Internet address family */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    server_addr.sin_port = htons(server_port);      /* Local port */

    /* Bind to the local address */
    if (bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        dieWithError("bind() failed");
    }

    /* Mark the socket so it will listen for incoming connections */
    if (listen(server_sock, MAXPENDING) < 0) {
        dieWithError("listen() failed");
    }

        /* Set the size of the in-out parameter */
    //clntLen = sizeof(echoClntAddr);
    //client_addr_size = sizeof(client_addr);
        /* Wait for a client to connect */
    while ((client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_addr_size)) >= 0) {
        printf("Handling client %s\n", inet_ntoa(client_addr.sin_addr));
        pthread_t worker;
        int sock = client_sock;
        // int *sock_ptr = malloc(sizeof(int));
        // *sock_ptr = clntSock;

        if(pthread_create(&worker, NULL, handle_connection, (void *)&sock)) {
            fprintf(stderr, "Failed to create a thread\n");
            close(sock);
            //free(sock_ptr);
        }
    }

}

bool handle_server_request(int server_sock, request *req, pthread_mutex_t *mutex_arr) {
    response res = { UNKNOWN, 0, 0, {0} };

    printf("Server request> %d %d (%d)<%s>\n", req->cmd, req->res, req->len, req->data);

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

    printf("Sending response> %d %d (%d)<%s>\n", res.status, res.lm, res.len, res.data);

    if(!send_response(server_sock, &res)) {
        fprintf(stderr, "Error sending response\n");
        return false;
    }

    fprintf(stderr, "Response sent\n");

    return true;
}

bool handle_client_request(int client_sock, int server_sock, request *req, pthread_mutex_t *mutex_arr) {
    response client_res = { UNKNOWN, 0, 0, {0} };
    response server_res = { UNKNOWN, 0, 0, {0} };

    fprintf(stderr, "Client request> %d %d (%d)<%s>\n", req->cmd, req->res, req->len, req->data);

    if(req->res < 0 || req->res > RESOURCE_MAX) {
        fprintf(stderr, "invalid resource: %d\n", req->res);
        return true;
    }

    if(req->cmd == SET) {
        bool set_local = set_resource(dir, req->res, req->data, &mutex_arr[req->res]);

        if(!set_local) {
            fprintf(stderr, "failed to set local resource\n");
            fill_response(&client_res, ERROR, "failed to set local resource");
            send_response(client_sock, &client_res);
            return false;
        }

        bool send_status = send_request(server_sock, req);

        if(!send_status) {
            fprintf(stderr, "failed to send set command to the second server\n");
            fill_response(&client_res, ERROR, "failed to send set command to the second server");
            send_response(client_sock, &client_res);
            return false;
        }

        bool set_status = receive_response(server_sock, &server_res);

        if(!set_status) {
            fprintf(stderr, "failed to receive response from second server\n");
            fill_response(&client_res, ERROR, "failed to receive response from second server");
            send_response(client_sock, &client_res);
            return false;
        }

        fprintf(stderr, "Sending response> %d %d (%d)<%s>\n",
            server_res.status, server_res.lm, server_res.len, server_res.data);

        if(!send_response(client_sock, &server_res)) {
            fprintf(stderr, "Failed to send server response to client\n");
            return false;
        }

        fprintf(stderr, "Response sent\n");
        return true;

    } else {
        fill_response(&client_res, ERROR, "Not implemented");
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

    if(!send_response(client_sock, &client_res)) {
        fprintf(stderr, "Error sending response\n");
        return false;
    }

    fprintf(stderr, "response sent\n");

    return true;
}

void *handle_connection(void *sock_ptr) {
    int sock = *((int *)sock_ptr);
    int second_server_sock;

    fprintf(stderr, "thread started\n");

    uint16_t id;
    if(!read_uint16(sock, &id)) {
        fprintf(stderr, "Connection failed to send identification\n");
        return 0;
    }

    fprintf(stderr, "id: %d\n", (int)id);

    if(id == CLIENT) {
        fprintf(stderr, "Trying to connect to the other server\n");
        struct sockaddr_in addr;
        second_server_sock = get_socket(second_server_ip, second_server_port, &addr, SERVER);

        if(second_server_sock < 0) {
            fprintf(stderr, "Could not connect to second server\n");
        }
    } else if(id == SERVER) {
        fprintf(stderr, "server connections not implemented\n");
    } else {
        fprintf(stderr, "Wrong ID\n");
        return 0;
    }

    request req = { UNKNOWN, 0, 0, {0} };
    while(read_request(sock, &req)) {
        if(id == CLIENT) {
            fprintf(stderr, "handling client request\n");
            if(!handle_client_request(sock, second_server_sock, &req, mutex_arr)) {
                break;
            }
        } else {
            fprintf(stderr, "handling server request\n");
            if(!handle_server_request(sock, &req, mutex_arr)) {
                break;
            }
        }
    }

    close(second_server_sock);
    close(sock);
    fprintf(stderr, "thread finished\n");
    return 0;
}
