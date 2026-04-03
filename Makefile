CC = cc
CFLAGS = -Wall -Wextra

all: build/main

build:
	mkdir -p build

build/main: build/main.o build/server.o build/commands.o build/str_utils.o
	$(CC) $(CFLAGS) $^ -o build/main

build/main.o: src/main.c src/server.h | build
	$(CC) $(CFLAGS) -c src/main.c -o $@

build/server.o: src/server.c src/server.h | build
	$(CC) $(CFLAGS) -c src/server.c -o $@

build/commands.o: src/commands.c src/commands.h src/server.h | build
	$(CC) $(CFLAGS) -c src/commands.c -o $@

build/str_utils.o: src/utils/str_utils.h src/utils/str_utils.c | build
	$(CC) $(CFLAGS) -c src/utils/str_utils.c -o $@

clean:
	rm -rf ./build
