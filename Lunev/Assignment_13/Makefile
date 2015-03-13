CC=gcc
CFLAGS=-c -Wall -MD -fprofile-arcs -ftest-coverage --coverage
LDFLAGS= -MD -fprofile-arcs -ftest-coverage --coverage
CPP_FILES := $(wildcard src/*.c)
H_FILES   := $(wildcard src/*.h)
D_FILES   := $(wildcard *.d)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.c=.o)))
EXECUTABLE = chained_hash_test

all: $(EXECUTABLE)

include $(D_FILES)


$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(LDFLAGS) $(OBJ_FILES) -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(EXECUTABLE) $(OBJ_FILES) $(D_FILES)


