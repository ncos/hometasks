CC=g++
CFLAGS=-c -Wall

SOURCES=listener.cpp talker.cpp functions.cpp
DEPS = config.h

OBJECTS=$(SOURCES:.cpp=.o)


all: clean $(SOURCES) listener talker
	rm -f $(OBJECTS)

talker: talker.o functions.o $(DEPS)
	$(CC) talker.o functions.o -o $@

listener: listener.o functions.o $(DEPS)
	$(CC) listener.o functions.o -o $@

.cpp.o: $(DEPS)
	$(CC) $(CFLAGS) $< -o $@



clean:
	rm -f listener talker $(OBJECTS)
	rm -f outfile

