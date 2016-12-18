CFLAGS=-Wall -Werror -std=c99 -pthread -I/home/tomas/Documents/backup_server/headers

client: client/client.c client/client_utils.c utils.c
	gcc $(CFLAGS) -o client.out client/client.c client/client_utils.c utils.c

server: server/server.c server/server_utils.c
	gcc $(CFLAGS) -o server.out server/server.c server/server_utils.c utils.c

%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)

clean:
	cd client && rm -f *.out
	cd server && rm -f *.out
