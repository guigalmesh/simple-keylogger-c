Um keylogger simples para Linux feito em C.

### Como usar

Compile com `make client` para o client e `make server` para o servidor. Ou então `make` para compilar os dois.

Rode o server_app na máquina atacante (de preferência com sudo).

O client_app recebe 2 argumentos, o primeiro o IP do servidor (não tem resolução DNS) e o segundo a porta do servidor na qual se conectar (por padrão esta setada como 4445, mas da pra alterar no header protocol.h).

Para testar localmente abra dois terminais, rode o `sudo ./server_app` em um e no outro rode `sudo ./client_app 127.0.0.1 4445`

O log vai ser armazenado no diretório onde o servidor foi rodado.

Por enquanto o servidor só aceita um cliente por vezes já que ele usa blocking e é single-threaded.

### Coisas que quero implementar no futuro

- Detectar letras em caps lock
- Detecção do padrão do teclado
- Usar o poll() para aceitar diversos clientes
- Armazenar os logs por cliente utilizando banco de dados
- Daemonização

### Créditos

Este projeto foi feito para estudar sobre o sistema o sistema Linux e comunicação cliente-servidor em C. Muito do projeto foi tirado desse repositório: [keylogger](https://github.com/cxnturi0n/keylogger)
