

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L

all: build

build:
	gcc $(CFLAGS) main.c client.c server.c keyboard.c printer.c list.c -o s-talk -lpthread


run: build
	./main

valgrind: build
	valgrind --leak-check=full ./main

clean:
	rm -f main
