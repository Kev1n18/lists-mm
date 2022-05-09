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
	gcovr -f singly_linked_list_mm.c

.PHONY: clean
clean:
	rm -f *.o *.gcda *.gcno main tests
