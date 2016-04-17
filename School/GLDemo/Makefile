CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lglut -lGL -lGLU -lGLEW
SOURCES=GLDemo.cpp GLfunctions.cpp
HEADERS=GLfunctions.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=gldemo

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o: $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -f *.o $(EXECUTABLE)
