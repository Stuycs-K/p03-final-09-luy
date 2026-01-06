.PHONY: client server clean
setup: client server
SRCS := $(wildcard *.c lib/*.c src/*.c)

client: client.o networking.o
	gcc -o client client.o networking.o
	./client

server: server.o networking.o
	gcc -o server server.o networking.o
	./server

client.o: client.c networking.h
	gcc -c client.c

server.o: server.c networking.h
	gcc -c server.c

networking.o: networking.c networking.h
	gcc -c networking.c

clean:
	@rm -rf *.o client server
