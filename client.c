#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int client_socket;
    struct sockaddr_in server_address;
    char message[500];
    char author[100];

    // Crie o socket do cliente
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("Erro ao criar o socket do cliente");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(15368); // Porta à qual o cliente vai se conectar
    // server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_addr.s_addr = inet_addr("54.94.248.37");


    // Conecte-se ao servidor
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Erro ao conectar ao servidor");
        exit(1);
    }

    printf("Conectado ao servidor. Digite o seu nome: ");
    fgets(author, sizeof(author), stdin);

    while (message[0] != 'e' || message[1] != 'x' || message[2] != 'i' || message[3] != 't')
    {
        printf("Digite a sua mensagem (ou 'exit' para sair): ");
        fgets(message, sizeof(message), stdin);

        // Verifique se o cliente quer sair
        if (strcmp(message, "exit\n") == 0)
        {
            break;
        }

        // Envie a mensagem e o autor separadamente para o servidor
        write(client_socket, message, sizeof(message));
        write(client_socket, author, sizeof(author));
    }

    // Feche o socket do cliente
    close(client_socket);

    return 0;
}
