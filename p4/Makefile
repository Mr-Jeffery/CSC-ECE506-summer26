CC     = gcc
CFLAGS = -Wall -ansi -pedantic -std=c99
LFLAGS = -fopenmp

p4: p4.c
	$(CC) $(CFLAGS) p4.c $(LFLAGS) -o p4

p4e: p4e.c
	$(CC) $(CFLAGS) p4e.c $(LFLAGS) -o p4e

clean:
	rm -f a.out p4 p4e
