CFLAGS = -Wall -pedantic -std=gnu99

.PHONY: 2310depot careful

careful: CFLAGS += -lm -g

2310depot: main.c depot.c depot.h
	gcc $(CFLAGS) main.c depot.c depot.h action.c -o 2310depot -lpthread
