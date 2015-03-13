#all: listener

#text: talker listener

#talker: talker/talker.cpp talker/talker.h talker/main.cpp config.h
#	g++ -g -Wall -o talker.bin main.cpp

#listener: listener.cpp listener.h main.cpp ../config.h
#	g++ -g -Wall -o listener.bin main.cpp





#clean:
#	$(RM) talker.bin
#	$(RM) listener.bin


CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=listener.cpp main.cpp
DEPS = listener.h ../config.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=listener

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) $(DEPS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o: $(DEPS)
	$(CC) $(CFLAGS) $< -o $@



.PHONY : clean
clean :
	-rm $(EXECUTABLE) $(OBJECTS)
