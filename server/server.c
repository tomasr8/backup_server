#include "server_utils.h"

#define MAXPENDING 5    /* Maximum requests */
pthread_mutex_t mutex_arr[RESOURCE_MAX + 1]; // 0-32 = 33
//pthread_mutex_t mutex_sigint;
char *dir; // path to resources
char *second_server_ip;
int second_server_port;
volatile sig_atomic_t sigint_sent = false; // CTRL-C

void signal_handler() {
    sigint_sent = true;
}

bool check_id(int id) {
    if(id != CLIENT && id != SERVER) {
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    int server_sock;
    int client_sock;
    struct sockaddr_in server_addr; /* local address */
    struct sockaddr_in client_addr; /* client address */
    unsigned short server_port;
    unsigned int client_addr_size = sizeof(client_addr);

    if (argc != 5) {
        log_err("Usage: <Server Port> <Second server port> <Second server address> <folder>\n");
        exit(1);
    }

    openlog(SERVER_LOGGER, LOG_PID, LOG_USER);
    signal(SIGPIPE, SIG_IGN);

    struct sigaction sa;
    // assign our handler function
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
      log_err("Error registering signal\n");
      return EXIT_FAILURE;
    }

    second_server_port = atoi(argv[2]);
    second_server_ip = argv[3];
    dir = argv[4];
    server_port = atoi(argv[1]);

    if(!check_resources(argv[4])) {
        log_err("failed to access required resources\n");
        return EXIT_FAILURE;
    }

    for(int i = 0; i < RESOURCE_MAX + 1; i++) {
        if(pthread_mutex_init(&mutex_arr[i], NULL) != 0) {
            log_err("Failed to create mutex no: %d\n", i);

            for(int j = 0; j < i; j++) {
                pthread_mutex_destroy(&mutex_arr[j]);
            }
            return EXIT_FAILURE;
        }
    }
    // if(pthread_mutex_init(&mutex_sigint, NULL) != 0) {
    //     log_err("Failed to create sigint mutex\n");
    //
    //     return EXIT_FAILURE;
    // }

    /* Create socket for incoming connections */
    if ((server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        log_err("socket() failed\n");
        return EXIT_FAILURE;
    }

    /* Construct local address structure */
    memset(&server_addr, 0, sizeof(server_addr));   /* Zero out structure */
    server_addr.sin_family = AF_INET;                /* Internet address family */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    server_addr.sin_port = htons(server_port);      /* Local port */

    /* Bind to the local address */
    if (bind(server_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        log_err("bind() failed");
        return EXIT_FAILURE;
    }

    /* Mark the socket so it will listen for incoming connections */
    if (listen(server_sock, MAXPENDING) < 0) {
        log_err("listen() failed");
        return EXIT_FAILURE;
    }

    while ((client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_addr_size)) >= 0) {
        if(sigint_sent) {
            log_info("CTRL-C received \n");
            break;
        }

        printf("Handling client %s\n", inet_ntoa(client_addr.sin_addr));
        pthread_t worker;
        int sock = client_sock;

        if(pthread_create(&worker, NULL, handle_connection, (void *)&sock)) {
            log_warn("Failed to create a thread\n");
            close(sock);
        }
    }

    log_info("Cleaning up\n");
    close(server_sock);
    for(int i = 0; i < RESOURCE_MAX; i++) {
        pthread_mutex_destroy(&mutex_arr[i]);
    }
    //pthread_mutex_destroy(&mutex_sigint);

    pthread_exit(0);
    //return EXIT_SUCCESS;
}

bool handle_server_request(int server_sock, request *req, pthread_mutex_t *mutex_arr) {
    response res = { UNKNOWN, 0, 0, {0} };

    log_debug("Server request> %d %d (%d)<%s>\n", req->cmd, req->res, req->len, req->data);

    if(req->res > RESOURCE_MAX) {
        log_warn("Invalid resource: %d\n", req->res);
        fill_response_err(&res, "Invalid resource");
        return send_response(server_sock, &res);
    }

    if(req->cmd == SET) {
        if(!set_resource(dir, req->res, req->data, &mutex_arr[req->res])) {
            log_warn("Failed to set resource\n");
            fill_response_err(&res, "Failed to set resource");
        } else {
            fill_response(&res, OK, "Success", LM_TOKEN);
        }
    } else if(req->cmd == GET) {
        char *path = path_join(dir, req->res);
        char buffer[MAX_SIZE + 1];
        uint32_t lm;

        const bool lm_ret = last_modified(path, &lm);
        const bool read_ret = read_file(path, buffer);

        free(path);

        if(!lm_ret) {
            log_warn("Error in stat()\n");
            fill_response_err(&res, "Error in stat()");
            return send_response(server_sock, &res);
        }

        if(!read_ret) {
            log_warn("Error reading file\n");
            fill_response_err(&res, "Error reading file");
            return send_response(server_sock, &res);
        }

        fill_response(&res, OK, buffer, lm);
    } else {
        fill_response_err(&res, "Not implemented");
    }

    log_debug("Sending response> %d %d (%d)<%s>\n", res.status, res.lm, res.len, res.data);

    if(!send_response(server_sock, &res)) {
        log_warn("Error sending response\n");
        return false;
    }

    log_debug("Response sent\n");

    return true;
}

bool handle_client_request(int client_sock, int server_sock, request *req, pthread_mutex_t *mutex_arr) {
    response client_res = { UNKNOWN, 0, 0, {0} }; // response that will be sent TO client
    response server_res = { UNKNOWN, 0, 0, {0} }; // response sent FROM the second server

    log_debug("Client request> %d %d (%d)<%s>\n", req->cmd, req->res, req->len, req->data);

    if(req->res > RESOURCE_MAX) {
        log_notice("Invalid resource: %d\n", req->res);
        fill_response_err(&client_res, "Invalid resource");
        return send_response(client_sock, &client_res); // non-fatal, connection will be kept alive if send_response succeeds
    }

    if(req->cmd == SET) {
        const bool set_local = set_resource(dir, req->res, req->data, &mutex_arr[req->res]);

        if(!set_local) {
            log_warn("Failed to set local resource\n");
            fill_response_err(&client_res, "failed to set local resource");
            return send_response(client_sock, &client_res); // again, connection can be kept alive
            //return false;
        }

        const bool send_status = send_request(server_sock, req);

        if(!send_status) {
            log_warn("Failed to send set command to the second server\n");
            fill_response_err(&client_res, "failed to send set command to the second server");
            send_response(client_sock, &client_res);
            return false; // something went really wrong, let's just kill this connection
        }

        const bool recv_status = receive_response(server_sock, &server_res);

        if(!recv_status) {
            log_warn("Could not receive response from second server\n");
            fill_response_err(&client_res, "failed to receive response from second server");
            send_response(client_sock, &client_res);
            return false; // same thing as above
        }

        log_debug("Sending response> %d %d (%d)<%s>\n",
            server_res.status, server_res.lm, server_res.len, server_res.data);

        if(!send_response(client_sock, &server_res)) {
            log_warn("Failed to send server response to client\n");
            return false;
        }

        log_debug("Response sent\n");
        return true;

    } else if(req->cmd == GET) {
        char *path = path_join(dir, req->res);
        char buffer[MAX_SIZE + 1];
        uint32_t lm;

        const bool lm_ret = last_modified(path, &lm);
        const bool read_ret = read_file(path, buffer);

        free(path);

        if(!lm_ret) {
            log_warn("Error in stat()\n");
            fill_response_err(&client_res, "Error in stat()");
            return send_response(client_sock, &client_res);
        }

        if(!read_ret) {
            log_warn("Error reading file\n");
            fill_response_err(&client_res, "Error reading file");
            return send_response(client_sock, &client_res);
        }

        if(!send_request(server_sock, req)) {
            log_warn("Error sending request to second server\n");
            fill_response_err(&client_res, "Error sending request to second server");
            return send_response(client_sock, &client_res);
        }

        if(!receive_response(server_sock, &server_res)) {
            log_warn("Error sending request to second server\n");
            fill_response_err(&client_res, "Error sending request to second server");
            send_response(client_sock, &client_res);
            return false;
        }

        //fprintf(stderr, "comparing lm's: %d %d\n", lm, server_res.lm);

        if(server_res.lm >= lm) {
            return send_response(client_sock, &server_res);
        } else {
            fill_response(&client_res, OK, buffer, lm);
            return send_response(client_sock, &client_res);
        }

    } else {
        fill_response_err(&client_res, "Not implemented");
    }

    if(!send_response(client_sock, &client_res)) {
        fprintf(stderr, "Error sending response\n");
        return false;
    }

    log_debug("Response sent\n");

    return true;
}

void *handle_connection(void *sock_ptr) {
    int sock = *((int *)sock_ptr);
    int second_server_sock;

    log_info("Thread started\n");

    uint16_t id;
    if(!read_uint16(sock, &id)) {
        log_warn("Connection failed to send identification\n");
        close(sock);
        log_info("Thread finished\n");
        return 0;
    }

    //fprintf(stderr, "id: %d\n", (int)id);
    if(!check_id(id)) {
        log_warn("Wrong ID\n");
        close(sock);
        log_info("Thread finished\n");
        return 0;
    }

    if(id == CLIENT) {
        log_debug("Trying to connect to the second server\n");
        struct sockaddr_in addr;
        second_server_sock = get_socket(second_server_ip, second_server_port, &addr, SERVER);

        if(second_server_sock < 0) {
            log_warn("Could not connect to the second server\n");
        }
    }

    request req = { UNKNOWN, 0, 0, {0} };
    while(!sigint_sent && read_request(sock, &req)) { // probably thread safe
        if(second_server_sock < 0 || id == SERVER) {
            log_debug("Handling request (no second server)\n");
            if(!handle_server_request(sock, &req, mutex_arr)) {
                break;
            }
        } else {
            log_debug("Handling request\n");
            if(!handle_client_request(sock, second_server_sock, &req, mutex_arr)) {
                break;
            }
        }
    }

    close(second_server_sock);
    close(sock);
    log_debug("Thread finished\n");
    return 0;
}
