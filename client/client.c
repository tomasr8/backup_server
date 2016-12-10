#include <stdlib.h>
#include "client_utils.h"

int main(int argc, char *argv[]) {
    struct sockaddr_in addr;
    char *IPs[2];
    int ports[2];
    int sock = -1;

    if(argc != 5) {
      fprintf(stderr, "Usage:\n");
      fprintf(stderr, "client <server-port> <server-ip-address> <backup-server-port> <backup-server-ip-address>\n");
      exit(1);
    }

    ports[0] = (int) strtol(argv[1], (char **)NULL, 10);
    ports[1] = (int) strtol(argv[3], (char **)NULL, 10);

    IPs[0] = argv[2];
    IPs[1] = argv[4];

    signal(SIGPIPE, SIG_IGN);

    sock = get_socket_multiple(IPs, ports, 2, &addr);

    if(sock < 0) {
        fprintf(stderr, "Failed to establish connection to server\n");
        return EXIT_FAILURE;
    }

    char buffer[LINE_SIZE];
    while(fgets(buffer, LINE_SIZE - 1, stdin) != NULL) {
        int status;

        if (buffer[strlen(buffer)-1] != '\n') {
            char ch;
            while (((ch = getchar()) != '\n') && (ch != EOF)) {}
        }

        status = parse_send_recv(sock, buffer);

        if(status == E_PARSE) {
            fprintf(stderr, "Error parsing command\n");
        } else if(status == E_CONNECTION) {
            fprintf(stderr, "Connection problem, reconnecting.. \n");

            do {
                sock = get_socket_multiple(IPs, ports, 2, &addr);
                status = parse_send_recv(sock, buffer);
            } while(status == E_CONNECTION && sock >= 0);
        }

    }

    // while(sock >= 0) {
    //     fprintf(stderr, "Connected to: %s on port %d, socket no. %d\n",
    //         inet_ntoa(addr.sin_addr), (int) ntohs(addr.sin_port), sock);
    //
    //     if(read_send_recv(sock)) {
    //         printf("EOF reached\n");
    //         return 0;
    //     }
    //
    //     sock = get_socket_multiple(IPs, ports, 2, &addr);
    // }
    //
    // fprintf(stderr, "Failed to establish connection to server\n");

    return 1;
}
