UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
COV=gcovr -f
else
COV=gcov -b
endif
CC=gcc
CFLAGS=-g -Wall -Wextra --coverage
OBJ=singly_linked_list_mm.o memory_manager.o
UNITY=unity/unity.c
TARGET=main

all: main

main: main.c $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

tests: tests.c $(OBJ) unity.o
	$(CC) $(CFLAGS) $^ -o $@

unity.o: $(UNITY)
	$(CC) -c $(CFLAGS) $^ -o $@

cov: clean tests
	./tests	
	$(COV) singly_linked_list_mm
.PHONY: clean
clean:
	rm -f *.o *.gcda *.gcno *.gcov main tests
