CC=g++
CFLAGS=-c -Wall -std=c++11 -g
LDFLAGS=-lglut -lGL -lGLU -lGLEW -lOpenCL -g
SOURCES=CLfunctions.cpp GLfunctions.cpp OpenCLPandA.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=oclp

all: info $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

info: info.o
	$(CC) info.o -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	@rm -f *.o $(EXECUTABLE) info
