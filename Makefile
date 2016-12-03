CFLAGS=-Wall -Werror -std=c99

client: client/client.o err.o ; gcc client/client.o err.o -o client/client.out $(CFLAGS)

server: server/server.o err.o ; gcc server/server.o err.o -o server/server.out $(CFLAGS)

%.o: %.c ; gcc -c -o $@ $< $(CFLAGS)

clean: ; rm -f *.o
