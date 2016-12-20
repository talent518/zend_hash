CC=gcc

CFLAGS = -O3
LFLAGS = 

all: hash

hash: zend_hash.lo main.lo
	$(CC) $(LFLAGS) -o $@ $?

%.lo: %.c
	$(CC) $(CFLAGS) -o $@ -c $?

clean:
	@rm -f *.lo hash
