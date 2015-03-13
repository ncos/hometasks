all: a.out
CFLAGS = -g -Wall -MD
OBJ = 1.o 2.o
a.out: $(OBJ)
	gcc -o $@ $^
clean:
	rm -rf $(OBJ) a.out *.d
include $(wildcard *.d)
