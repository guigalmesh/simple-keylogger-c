#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"

void must_init(const char *message, int errcode){
    if(errcode < 0){
        perror(message);
        exit(EXIT_FAILURE);
    }
}