CC = g++
CFLAGS = -Wall

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: thermal

thermal: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@.bin


%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@



.PHONY: clean

clean:
	rm -f thermal.bin

