

CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=listener.cpp main.cpp
DEPS = listener.h ../config.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=listener

all: clean $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(DEPS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o: $(DEPS)
	$(CC) $(CFLAGS) $< -o $@



.PHONY : clean
clean :
	-rm $(EXECUTABLE) $(OBJECTS)
	-rm outfile
