#include "client_utils.h"

int main(int argc, char *argv[]) {
    struct sockaddr_in addr;
    char *IPs[2];
    int ports[2];
    int sock = -1;

    openlog(CLIENT_LOGGER, LOG_PID, LOG_USER);
    signal(SIGPIPE, SIG_IGN); // potentially unsafe, #TODO change to sigaction()

    if(argc != 5) {
      fprintf(stderr, "Usage:\n");
      fprintf(stderr, "client <server-port> <server-ip-address> <backup-server-port> <backup-server-ip-address>\n");
      closelog();
      return EXIT_FAILURE;
    }

    ports[0] = (int) strtol(argv[1], (char **)NULL, 10); // safer variant of atoi
    ports[1] = (int) strtol(argv[3], (char **)NULL, 10);

    IPs[0] = argv[2];
    IPs[1] = argv[4];

    sock = get_socket_multiple(IPs, ports, 2, &addr, CLIENT);

    if(sock < 0) {
        log_warn("Failed to establish connection to server\n");
        closelog();
        return EXIT_FAILURE;
    }

    char buffer[LINE_SIZE];
    while(fgets(buffer, LINE_SIZE - 1, stdin) != NULL) {

        // if the last character is not a new line, keep reading until it is,
        // thus effectively throwing the rest of line away
        if (buffer[strlen(buffer)-1] != '\n') {
            char ch;
            while (((ch = getchar()) != '\n') && (ch != EOF)) {}
        }
        //fprintf(stderr, "waiting for input..\n");
        //fprintf(stderr, "> ");
        int status = parse_send_recv(sock, buffer);

        if(status == E_PARSE) {
            log_notice("Error parsing command\n");
        } else if(status == E_CONNECTION) {
            log_notice("Connection problem, reconnecting.. \n");

            do {
                sock = get_socket_multiple(IPs, ports, 2, &addr, CLIENT);

                if(sock < 0) {
                    log_info("Could not reconnect. Stopping client\n");
                    closelog();
                    return EXIT_FAILURE;
                }

                log_debug("Reconnected socket no: %d\n", sock);
                //fprintf(stderr, "> ");
                status = parse_send_recv(sock, buffer);

            } while(status == E_CONNECTION && sock >= 0);
        }

    }

    log_info("End of input reached\n");
    closelog();
    return EXIT_SUCCESS;
}
