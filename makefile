.PHONY: client server clean
setup: client server
	
client: client.o networking.o
	gcc -o client client.o networking.o

server: server.o networking.o
	gcc -o server server.o networking.o

client.o: client.c networking.h
	gcc -c client.c

server.o: server.c networking.h
	gcc -c server.c

networking.o: networking.c networking.h
	gcc -c networking.c

clean:
	@rm -rf *.o client server
