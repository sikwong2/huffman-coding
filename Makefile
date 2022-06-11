CC = clang
CFLAGS = -g -Wall -Wextra -Werror -Wpedantic

all: encode decode
encode: encode.o code.o stack.o pq.o node.o io.o huffman.o
	$(CC) -g -o encode encode.o code.o stack.o pq.o node.o io.o huffman.o

decode: decode.o code.o stack.o pq.o node.o io.o huffman.o
	$(CC) -g -o decode decode.o code.o stack.o pq.o node.o io.o huffman.o

huffman.o: huffman.c 
	$(CC) $(CFLAGS) -c huffman.c

io: io.o code.o
	$(CC) -g -o io io.o code.o

io.o: io.c
	$(CC) $(CFLAGS) -c io.c

code.o: code.c
	$(CC) $(CFLAGS) -c code.c

stack.o: stack.c
	$(CC) $(CFLAGS) -c stack.c

pq.o: pq.c
	$(CC) $(CFLAGS) -c pq.c

node.o: node.c
	$(CC) $(CFLAGS) -c node.c

clean:
	rm -f encode decode *.o

format:
	clang-format -i -style=file *.[c,h]

scan-build: clean
	scan-build --use-cc=$(CC) make
