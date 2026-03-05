# Compilador e flags de compilação
CC = gcc
CFLAGS = -Wall -Wextra

# Configurações do Cliente (Keylogger)
CLIENT_DIR = keylogger-client
CLIENT_SRC = $(CLIENT_DIR)/src/main.c $(CLIENT_DIR)/src/keylogger.c
CLIENT_INC = -I$(CLIENT_DIR)/include
CLIENT_BIN = client_app

# Configurações do Servidor
SERVER_DIR = keylogger-server
SERVER_SRC = $(SERVER_DIR)/server.c
SERVER_BIN = server_app

# Alvo padrão: compila tudo quando você digita apenas 'make'
all: client server

SHARED_INC = -Ishared

# Regra para compilar o cliente
client:
	$(CC) $(CFLAGS) $(CLIENT_INC) $(SHARED_INC) $(CLIENT_SRC) -o $(CLIENT_BIN)

# Regra para compilar o servidor
server:
	$(CC) $(CFLAGS) $(SHARED_INC) $(SERVER_SRC) -o $(SERVER_BIN)

# Regra para limpar os arquivos compilados (útil para recompilar do zero)
clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN)

# Declara que 'all', 'client', 'server' e 'clean' não são nomes de arquivos
.PHONY: all client server clean
