EXEC=xomach00
CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -DNDEBUG

SRCS=$(wildcard *.c)
HEADERS=$(wildcard *.h)
OBJS=$(SRCS:%.c=%.o)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf *.o
	rm $(EXEC)

pack: $(SRCS) $(HEADERS) Makefile rozdeleni rozsireni
	zip $(EXEC).zip $^

