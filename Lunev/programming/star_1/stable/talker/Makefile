CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=talker.cpp main.cpp
DEPS = talker.h ../config.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=talker

all: clean $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(DEPS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o: $(DEPS)
	$(CC) $(CFLAGS) $< -o $@

.PHONY : clean
clean :
	-rm $(EXECUTABLE) $(OBJECTS)
