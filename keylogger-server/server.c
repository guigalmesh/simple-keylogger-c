#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <shared/protocol.h>

#include "server.h"

void fill_server_addr(struct sockaddr_in *server_sockaddr){
    server_sockaddr->sin_family = PF_INET; // IPv4
    server_sockaddr->sin_port = SERVER_PORT; // define a porta
    server_sockaddr->sin_addr.s_addr = htonl(INADDR_ANY); // servidor vai aceitar conexões de qualquer interface
    memset(server_sockaddr->sin_zero, 0, 8); // padding para encher os 8 bytes restantes

}

int main(){
    // cria o ponto de comunicação inicial, devolve um file descriptor
    int server_socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    must_init("socket() failed\n", server_socketFd);

    struct sockaddr_in server_sockAddr;
    fill_server_addr(&server_sockAddr);
    
    // passamos a struct para bindar o socket a uma porta
    must_init("bind() failed\n", bind(server_socketFd, (struct sockaddr*)&server_sockAddr, sizeof(server_sockAddr)));

    // informa ao OS que este socket será passivo, também define um backlog que é o tamanho da fila de espera
    must_init("listen() failed\n", listen(server_socketFd, BACKLOG));


    struct sockaddr_in client_sockAddr;
    socklen_t client_addrLen = sizeof(client_sockAddr);

    // bloqueia a execução até um cliente se conectar, então cria e devolve um novo file descriptor
    // é por esse file descriptor que a comunicação vai acontecer, o outro file descriptor fica livre
    // para receber novas conexões
    int client_socketFd = accept(server_socketFd, (struct sockaddr_in*)&client_sockAddr, &client_addrLen);
    must_init("accept() failed\n", client_socketFd);
    
}