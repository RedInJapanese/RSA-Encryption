CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LFLAGS = $(shell pkg-config --libs gmp)
SORUCES = randstate.c numtheory.c rsa.c
EXEC = test
OBJECTS = $(SOURCES)

all: keygen encrypt decrypt

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) $(LFLAGS) -g -o keygen $(SOURCES) keygen.c
	$(CC) $(CFLAGS) $(LFLAGS) -g -o encrypt $(SOURCES) encrypt.c
	$(CC) $(CFLAGS) $(LFLAGS) -g -o decrypt $(SOURCES) decrypt.c


%.o:%.c
	$(CC) $(CFLAGS) -c $<

tidy:
	rm -rf $(OBJECTS)

clean: tidy
	rm -rf keygen encrypt decrypt

format: 
	clang-format -i -style=file *.[c,h]

keygen:
	$(CC) $(CFLAGS) $(LFLAGS) -lm -g -o keygen rsa.c numtheory.c randstate.c keygen.c

encrypt:
	$(CC) $(CFLAGS) $(LFLAGS) -lm -g -o encrypt rsa.c numtheory.c randstate.c encrypt.c

decrypt: 
	$(CC) $(CFLAGS) $(LFLAGS) -lm -g -o decrypt rsa.c numtheory.c randstate.c decrypt.c

pkg-config:
	sudo apt install pkg-config libgmp-dev

