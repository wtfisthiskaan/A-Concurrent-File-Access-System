all: biboServer biboClient

biboServer: server.o
	gcc server.o -o biboServer

server.o: server.c
	gcc -c server.c

biboClient: client.o
	gcc client.o -o biboClient

client.o: client.c
	gcc -c client.c

clean:
	rm -rf *o biboServer biboClient 