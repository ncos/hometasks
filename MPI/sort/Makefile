CC = mpiCC
CFLAGS = -Wall

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)

all: sort

sort: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@.bin

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@


.PHONY: clean

clean:
	rm -f sort.bin
	rm -f *.o
