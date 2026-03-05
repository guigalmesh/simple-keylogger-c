#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void fill_server_addr(struct sockaddr_in *server_sockaddr);