CC=g++
CFLAGS=-c -Wall -std=c++11
LDFLAGS=-lglut -lGL -lGLU -lGLEW -lOpenCL
SOURCES=CLfunctions.cpp GLfunctions.cpp OpenCLParticle.cpp info.cpp opencl_demo.cpp
HEADERS=CLfunctions.h GLfunctions.h pandatype.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=oclp

all: info demo $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): CLfunctions.o GLfunctions.o OpenCLParticle.o $(HEADERS)
	$(CC) CLfunctions.o GLfunctions.o OpenCLParticle.o -o $@ $(LDFLAGS)

info: info.o
	$(CC) info.o -o $@ $(LDFLAGS)

demo: opencl_demo.o
	$(CC) opencl_demo.o -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -f *.o $(EXECUTABLE) info demo
