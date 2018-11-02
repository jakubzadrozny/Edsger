CC = gcc
CFLAGS = -lm -std=c99 `pkg-config gtk+-3.0 --cflags --libs`

project: main.c interface.c
	$(CC) -o Edsger main.c interface.c graph.c list.c saving.c $(CFLAGS)
