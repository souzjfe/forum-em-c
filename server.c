#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

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
  pthread_t thread_id;
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
    ssize_t read_bytes = read(client_socket, newMessage.text, sizeof(newMessage.text));
    if (read_bytes <= 0)
    {
      newMessage.author[strlen(newMessage.author) - 1] = '\0'; // Remova o '\n' do final da string
      printf("%s se desconectou.\n", newMessage.author);
      break; // Encerre a thread em caso de erro ou desconexão
    }

    if (read(client_socket, newMessage.author, sizeof(newMessage.author)) == -1)
    {
      perror("Erro ao ler o autor da mensagem do cliente");
      break; // Encerre a thread em caso de erro
    }

    // Obter a data atual
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(newMessage.date, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Armazene a nova mensagem na estrutura de dados

    printf("\nMensagens no fórum:\n");
    
    printf("Data: %s\n", newMessage.date);
    newMessage.author[strlen(newMessage.author) - 1] = '\0'; 
    printf("%s: %s\n\n",newMessage.author, newMessage.text);
    
  }

  // Feche o socket do cliente
  close(client_socket);
  pthread_cancel(thread_args->thread_id); // Encerre a própria thread
  return NULL;
}

int main()
{
  int server_socket;
  struct sockaddr_in server_address;
  socklen_t client_address_length = sizeof(struct sockaddr_in);

  // Crie o socket do servidor
  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket == -1)
  {
    perror("Erro ao criar o socket do servidor");
    exit(1);
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(9002); 
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

    if (pthread_create(&threads[i], NULL, handle_client, &thread_args) != 0)
    {
      perror("Erro ao criar a thread");
      exit(1);
    }

    thread_args.thread_id = threads[i];
  }

  for (int i = 0; i < 10; i++)
  {
    pthread_join(threads[i], NULL);
  }

  // Feche o socket do servidor
  close(server_socket);

  return 0;
}
