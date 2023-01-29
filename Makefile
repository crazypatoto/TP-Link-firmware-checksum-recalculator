CC = gcc
INCLUDE = -I.
LIB = -lssl -lcrypto

.PHONY : all
all: checksum

checksum: checksum.c
	$(CC) $^ $(INCLUDE) $(LIB) -o $@

.PHONY : clean
clean: 
	rm -f checksum