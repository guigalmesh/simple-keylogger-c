# Compilador e flags de compilação
CC = gcc
CFLAGS = -Wall -Wextra

# Pasta compartilhada
SHARED_DIR = shared
SHARED_SRC = $(SHARED_DIR)/protocol.c
SHARED_INC = -I$(SHARED_DIR)

# Configurações do Cliente (Keylogger)
CLIENT_DIR = keylogger-client
CLIENT_SRC = $(CLIENT_DIR)/src/main.c $(CLIENT_DIR)/src/keylogger.c $(SHARED_SRC)
CLIENT_INC = -I$(CLIENT_DIR)/include
CLIENT_BIN = client_app

# Configurações do Servidor
SERVER_DIR = keylogger-server
SERVER_SRC = $(SERVER_DIR)/server.c $(SHARED_SRC)
SERVER_BIN = server_app

# Alvo padrão
all: client server

# Regra para compilar o cliente
client:
	$(CC) $(CFLAGS) $(CLIENT_INC) $(SHARED_INC) $(CLIENT_SRC) -o $(CLIENT_BIN)

# Regra para compilar o servidor
server:
	$(CC) $(CFLAGS) $(SHARED_INC) $(SERVER_SRC) -o $(SERVER_BIN)

# Limpeza
clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN)

.PHONY: all client server clean