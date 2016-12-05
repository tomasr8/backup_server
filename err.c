#include <stdio.h> /* for perror() */
#include <stdlib.h> /* for exit() */
#include "err.h"

void dieWithError(const char *errorMessage) {
    perror (errorMessage);
    exit(1);
}
