CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=Sudoku.cpp Sudoku_test.cpp
HEADERS=Sudoku.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sudoku

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o: $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -f *.o $(EXECUTABLE)
