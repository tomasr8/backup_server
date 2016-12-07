#include <stdio.h>
#include <sys/socket.h>
#include <stdint.h>
#include "err.h"
#include "commands.h"
#include "utils.h"

bool receive_response(int socket, response *res) {

    if(!read_uint8(socket, &res->status)) {
        return false;
        //dieWithError("Error reading command\n");
    }

    if(!read_uint8(socket, &res->len)) {
        return false;
        //dieWithError("Error reading data length\n");
    }

    if(res->len == 0) {
        //printf("Received response from server: Status code: %d\n", res.status);
        return true;
    }

    if(!readString(socket, res->data, res->len)) {
        return false;
        //dieWithError("Error reading data\n");
    }

      //printf("Received response from server: Status code: %d\n", res.status);
      //printf("Data: %s\n", res.data);
    return true;
}
