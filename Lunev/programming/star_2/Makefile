CC=g++
CFLAGS=-c -Wall

SOURCES=star_2.cpp
DEPS = star_2.h

OBJECTS=$(SOURCES:.cpp=.o)


all: clean receiver.cpp sender.cpp star_2.cpp star_2

star_2: star_2.o $(DEPS)
	$(CC) star_2.o -o $@

.cpp.o: $(DEPS)
	$(CC) $(CFLAGS) $< -o $@



clean:
	rm -f star_2 $(OBJECTS)

