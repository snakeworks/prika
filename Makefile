CC = cc
CFLAGS = -Wall -Wextra

all: build/main

build:
	mkdir -p build

build/main: build/main.o build/str_utils.o
	$(CC) $(CFLAGS) $^ -o build/main

build/main.o: src/main.c | build
	$(CC) $(CFLAGS) -c src/main.c -o $@

build/str_utils.o: src/utils/str_utils.h src/utils/str_utils.c | build
	$(CC) $(CFLAGS) -c src/utils/str_utils.c -o $@

clean:
	rm -rf ./build
