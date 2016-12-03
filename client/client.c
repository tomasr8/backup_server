#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "err.h"

#define RCVBUFFSIZE 32

void dieWithError(char *errorMessage);

int main(int argc, char *argv[]) {

  int sock;
    struct sockaddr_in echoServAddr;
    unsigned short echoServPort;
    char *servIP;
    char *echoString;
    char echoBuffer[RCVBUFFSIZE];
    unsigned int echoStringLen;
    int bytesRcvd, totalBytesRcvd;

    if((argc < 3) || (argc > 4)) {
      fprintf(stderr, "Wrong number of arguments");
      exit(1);
    }

    servIP = argv[1];
    echoString = argv[2];

    if(argc == 4) {
      echoServPort = atoi(argv[3]);
    } else {
      echoServPort = 7;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      dieWithError("socket() failed");
    }

    memset(&echoServAddr, 0, sizeof(echoServAddr));
    echoServAddr.sin_family = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port = htons(echoServPort);

    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
      dieWithError("connect() failed");
    }

    echoStringLen = strlen(echoString);

    if (send(sock, echoString, echoStringLen, 0) != echoStringLen) {
      dieWithError("send() sent different number of bytes");
    }

    totalBytesRcvd = 0;
    printf("Received: ");

    while(totalBytesRcvd < echoStringLen) {
      if ((bytesRcvd = recv(sock, echoBuffer, RCVBUFFSIZE - 1, 0)) <= 0) {
        dieWithError("recv() failed or connection closed prematurely");
      }

      totalBytesRcvd += bytesRcvd;
      echoBuffer[bytesRcvd] = '\0';
      printf("%s", echoBuffer);
    }

    printf("\n");

    close(sock);
    return 0;
}
