all: client server

client: client.o networking.o
	gcc -o client client.o networking.o

server: server.o networking.o user.o
	gcc -o server server.o networking.o user.o

client.o: client.c networking.h
	gcc -c client.c

server.o: server.c networking.h user.h
	gcc -c server.c

networking.o: networking.c networking.h
	gcc -c networking.c

user.o: user.c
	gcc -c user.c

clean:
	rm server
	rm client
	rm *.o
	rm *~

data-clean:
	rm data/pages/*
