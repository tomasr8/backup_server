#include <stdio.h> /* for perror() */
#include <stdlib.h> /* for exit() */

void dieWithError(char *errorMessage) {
    perror (errorMessage);
    exit(1);
}
