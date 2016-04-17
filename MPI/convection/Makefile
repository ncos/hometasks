CC = mpic++
CFLAGS = -Wall

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: convection

convection: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@.bin


%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@



.PHONY: clean

clean:
	rm -f convection.bin
	rm -f convection.ppm
