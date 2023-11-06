#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

// Estrutura para representar uma mensagem no fórum
struct Message
{
  char author[100];
  char text[500];
  char date[20];
};

struct ThreadArgs
{
  int client_socket;
  struct Message *forum;
};

void *handle_client(void *args)
{
  struct ThreadArgs *thread_args = (struct ThreadArgs *)args;
  int client_socket = thread_args->client_socket;
  struct Message *forum = thread_args->forum;

  char message[500];
  char author[100];
  struct Message newMessage;

  while (1)
  {
    // Leia a mensagem do cliente
    if (read(client_socket, newMessage.text, sizeof(newMessage.text)) == -1)
    {
      perror("Erro ao ler a mensagem do cliente");
      break;
    }

    // Leitura do autor da mensagem do cliente
    if (read(client_socket, newMessage.author, sizeof(newMessage.author)) == -1)
    {
      perror("Erro ao ler o autor da mensagem do cliente");
      break;
    }

    // Obter a data atual
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(newMessage.date, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Armazene a nova mensagem na estrutura de dados
    forum[0] = newMessage;

    printf("\nMensagens no fórum:\n");
    for (int i = 0; i < 100; i++)
    {
      if (forum[i].text[0] == '\0')
      {
        break;
      }
      printf("Autor: %s\n", forum[i].author);
      printf("Data: %s\n", forum[i].date);
      printf("Mensagem: %s\n", forum[i].text);
    }
  }

  // Feche o socket do cliente
  close(client_socket);
  return NULL;
}

int main()
{
  int server_socket;
  struct sockaddr_in server_address;
  socklen_t client_address_length = sizeof(struct sockaddr_in);

  // Crie o socket do servidor
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1)
  {
    perror("Erro ao criar o socket do servidor");
    exit(1);
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(9002); // Porta que o servidor vai escutar
  server_address.sin_addr.s_addr = INADDR_ANY;

  // Faça o bind do socket do servidor
  if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
  {
    perror("Erro ao fazer o bind");
    exit(1);
  }

  // Coloque o servidor no modo de escuta
  if (listen(server_socket, 10) == -1)
  {
    perror("Erro ao escutar");
    exit(1);
  }

  printf("Servidor aguardando conexões...\n");

  struct Message forum[100];
  pthread_t threads[10]; // Suporta até 10 conexões simultâneas

  for (int i = 0; i < 10; i++)
  {
    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket == -1)
    {
      perror("Erro ao aceitar a conexão do cliente");
      exit(1);
    }

    struct ThreadArgs thread_args;
    thread_args.client_socket = client_socket;
    thread_args.forum = forum;

    pthread_create(&threads[i], NULL, handle_client, &thread_args);
  }

  for (int i = 0; i < 10; i++)
  {
    pthread_join(threads[i], NULL);
  }

  // Feche o socket do servidor
  close(server_socket);

  return 0;
}
