CC=gcc
CFLAGS=-fsanitize=address -Wall -Werror -std=gnu11 -g -lm

tests: tests.c virtual_alloc.c
	$(CC) $^ -o $@ $(CFLAGS)
clean:
	rm tests
# tests_og: tests_og.c og_1.c
# 	$(CC) $(CFLAGS) $^ -o $@