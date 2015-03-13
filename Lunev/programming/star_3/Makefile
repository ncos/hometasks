CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=child.cpp main.cpp
DEPS = child.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=proxy

all: clean $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(DEPS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o: $(DEPS)
	$(CC) $(CFLAGS) $< -o $@

.PHONY : clean
clean :
	-rm $(EXECUTABLE) $(OBJECTS)
