#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "commands.h"
#include "err.h"
#include "utils.h"
#include "client_utils.h"

#define LINE_SIZE 264

void read_send_recv(socklen_t sock);

int main(int argc, char *argv[]) {
    struct sockaddr_in primary_addr;
    char *primaryIP;
    int primary_port;
    socklen_t sock;

    if(argc != 3) {
      fprintf(stderr, "Usage:\n");
      fprintf(stderr, "client <server-port> <server-ip-address> <backup-server-port> <backup-server-ip-address>\n");
      exit(1);
    }

    primary_port = (int) strtol(argv[1], (char **)NULL, 10);
    //secondary_port = (int) strtol(argv[3], (char **)NULL, 10);

    primaryIP = argv[2];
    //secondaryIP = argv[4];

    printf("%d %s\n", primary_port, primaryIP);

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 ) {
        dieWithError("socket() failed");
    }

    memset(&primary_addr, 0, sizeof(primary_addr));
    primary_addr.sin_family = AF_INET;
    primary_addr.sin_addr.s_addr = inet_addr(primaryIP);
    primary_addr.sin_port = htons(primary_port);

    if (connect(sock, (struct sockaddr *) &primary_addr, sizeof(primary_addr)) < 0) {
       dieWithError("connect() failed");
    }

    read_send_recv(sock);

    //close(sock);

    return 0;
}

void read_send_recv(socklen_t sock) {
    char buffer[LINE_SIZE];

    while(fgets(buffer, LINE_SIZE - 1, stdin) != NULL) {

        if (buffer[strlen(buffer)-1] != '\n') {
            char ch;
            while (((ch = getchar()) != '\n') && (ch != EOF)) {}
        }

        printf("buffer length: %d\n", (int)strlen(buffer));

        request req = { UNKNOWN, 0, 0, {0} };

        if(!parseRequest(buffer, &req)) {
            fprintf(stderr, "Error parsing command\n");
            continue;
        }

        printf("to Send> cmd:%d resource:%d (%d bytes)<%s>\n", req.cmd, req.res, req.len, req.data);

      //char msg[] = "Hello world";
    //    char buff[256];

    //   if(send(sock, msg, strlen(msg), 0) <= 0) {
    //       fprintf(stderr, "send failed\n");
    //   }

        // uint16_t num = 42;
        //
        // num = htons(num);
        // printf("sending %d\n", num);
        // if(send(sock, &num, sizeof(uint16_t), 0) <= 0) {
        //     printf("Failed to send number\n");
        // }
        // printf("sent number\n");
        //
        // if(recv(sock, buff, 255, 0) == 0) {
        //     fprintf(stderr, "recv failed");
        // }
        //
        // printf("<%s>\n", buff);

      if(!send_request(sock, &req)) {
          fprintf(stderr, "Error sending request, trying to connect to other server.. \n");
      }

      response res = { UNKNOWN, 0, {0} };

      if(!receive_response(sock, &res)) {
          fprintf(stderr, "Error reading response.. \n");
      }

      printf("Response> %d (%d)<%s>\n", res.status, res.len, res.data);

    }
}
