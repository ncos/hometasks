CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lglut -lGL -lGLU -lGLEW -lOpenCL
SOURCES=CLfunctions.cpp GLfunctions.cpp OpenCLPandA.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=oclp

all: info $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

info: info.cpp
	$(CC) info.cpp -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	@rm -f *.o $(EXECUTABLE) info
