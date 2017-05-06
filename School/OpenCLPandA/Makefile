CC=g++
CFLAGS=-c -Wall -std=c++11 -g
LDFLAGS=-lglut -lGL -lGLU -lGLEW -lOpenCL -g
SOURCES=CLfunctions.cpp GLfunctions.cpp OpenCLPandA.cpp info.cpp opencl_demo.cpp
HEADERS=CLfunctions.h GLfunctions.h pandatype.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=oclp

all: info demo $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): CLfunctions.o GLfunctions.o OpenCLPandA.o $(HEADERS)
	$(CC) CLfunctions.o GLfunctions.o OpenCLPandA.o -o $@ $(LDFLAGS)

info: info.o
	$(CC) info.o -o $@ $(LDFLAGS)

demo: opencl_demo.o
	$(CC) opencl_demo.o -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -f *.o $(EXECUTABLE) info demo
