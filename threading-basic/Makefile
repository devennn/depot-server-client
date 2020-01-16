CFLAGS = -Wall -pedantic -g -std=gnu99 -pthread

.PHONY: all 

careful: CFLAGS += -lm

all : sum_single sum_multiple sum_malloc

sum_single: sum_single_thread.c
	gcc $(CFLAGS) sum_single_thread.c -o sum_single

sum_multiple: sum_multiple_thread.c
	gcc $(CFLAGS) sum_multiple_thread.c -o sum_multiple

sum_malloc: sum_malloc_thread.c
	gcc $(CFLAGS) sum_malloc_thread.c -o sum_malloc
