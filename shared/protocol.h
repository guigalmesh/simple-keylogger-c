#include <stdbool.h>
#include <arpa/inet.h>

#define SERVER_PORT 7777
#define BACKLOG 10
#define DEVICES_PATH "/dev/input/"

typedef struct{
    int key_code; // código bruto da tecla (ex: 30 = A)
    bool is_pressed; // 1 pressionada 0 solta
} KeyPackage;

void must_init(const char *message, int errcode);