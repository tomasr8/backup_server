CFLAGS=-Wall -Werror -std=c99 -pthread -I/home/tomas/Documents/backup_server/headers

serv: test_server_utils.c
	gcc $(CFLAGS) -I../headers -I../lib/unity/src/ \
	../utils.c test_server_utils.c ../server/server_utils.c \
	../lib/unity/src/unity.c -o test_server_utils.out

cl: test_client_utils.c
	gcc $(CFLAGS) -I../headers -I../lib/unity/src/ \
	../utils.c test_client_utils.c ../client/client_utils.c \
	../lib/unity/src/unity.c -o test_client_utils.out

sock: test_socket_functions.c
	gcc $(CFLAGS) -I../headers -I../lib/unity/src/ \
	test_socket_functions.c ../utils.c \
	../lib/unity/src/unity.c -o test_socket_functions.out

io: test_io.c
	gcc $(CFLAGS) -I../headers -I../lib/unity/src/ \
	test_io.c ../utils.c \
	../lib/unity/src/unity.c -o test_io.out

clean:
	rm -f *.out
