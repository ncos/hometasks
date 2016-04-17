CC = g++
CFLAGS = -Wall

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: brussel

brussel: $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@.bin


%.o: %.cpp definitions.h
	$(CC) -c $(CFLAGS) $< -o $@



.PHONY: clean

clean:
	rm -f brussel.bin

