CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lglut -lGL -lGLU -lGLEW
SOURCES=main.cpp World_class.cpp GLfunctions.cpp
HEADERS=GLfunctions.h World_class.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=glexplosion

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o: $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -f *.o $(EXECUTABLE)
