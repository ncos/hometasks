CC=g++
CFLAGS=-c -Wall -g
LDFLAGS= -g
SOURCES=File_sort.cpp Divide.cpp File_sort_functions.cpp Merge.cpp
HEADERS=Divide.h File_sort_functions.h Merge.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=psort

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o: $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -f *.o $(EXECUTABLE)
