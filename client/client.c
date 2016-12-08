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

    //printf("%d %s %d %s\n", ports[0], IPs[0], ports[1], IPs[1]);

    for(int i = 0; i < 2; i++) {
        if((sock = get_socket(IPs[i], ports[i], &addr)) >= 0) {
            break;
        }
    }

    if(sock < 0) {
        dieWithError("Failed to establish connection to server\n");
    }

    fprintf(stderr, "Connected to: %s on port %d, socket no. %d\n",
        inet_ntoa(addr.sin_addr), (int) ntohs(addr.sin_port), sock);

    read_send_recv(sock);

    close(sock);

    return 0;
}
