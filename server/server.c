#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "commands.h"
#include "utils.h"
#include "server_utils.h"

#define MAXPENDING 5    /* Maximum outstanding connection requests */

void HandleTCPClient(int sock);   /* TCP client handling function */

int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */

    if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for incoming connections */
    if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        dieWithError("socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        dieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        dieWithError("listen() failed");

        /* Set the size of the in-out parameter */
    clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
    if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr,
                           &clntLen)) < 0)
        dieWithError("accept() failed");

    /* clntSock is connected to a client! */

    printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

    HandleTCPClient(clntSock);
}

void HandleTCPClient(int sock) {
    response res = { OK, 0, {0} };
    char msg[] = "Success";
    strcpy(res.data, msg);
    res.len = strlen(msg);

    while(1) {
        //char buffer[256];

        /*if(recv(sock, buffer, 255, 0) == 0) {
            fprintf(stderr, "recv failed");
        }*/

        // uint16_t num;
        // if(!read_uint16(sock, &num)) {
        //     fprintf(stderr, "failed to read number\n");
        // }
        //
        // printf("<%d>\n", num);
        //
        // if(send(sock, msg, strlen(msg), 0) <= 0) {
        //     fprintf(stderr, "send failed");
        // }

        request req = { UNKNOWN, 0, 0, {0} };

        if(!receive_request(sock, &req)) {
            fprintf(stderr, "Error reading request\n");
            break;
        }

        printf("Request> %d %d (%d)<%s>\n", req.cmd, req.res, req.len, req.data);

        if(!send_response(sock, &res)) {
            fprintf(stderr, "Error sending response\n");
            break;
        }

    }

    close(sock);    /* Close client socket */
}
