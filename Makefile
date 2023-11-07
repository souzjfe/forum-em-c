CC = gcc
CFLAGS = -Wall

SERVER_SRC = server.c
CLIENT_SRC = client.c

all: server client

server: $(SERVER_SRC)
	$(CC) $(CFLAGS) $(SERVER_SRC) -o server && ./server

client: $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o client && ./client

clean:
	rm -f server client
