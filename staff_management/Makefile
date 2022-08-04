CC=gcc
DEBUG=-g -O0 -Wall 
CFLAGS += $(DEBUG)

OBJS=${patsubst %.c, %, ${wildcard *.c}}

all: $(OBJS)

% : %.c
	$(CC) $(DEBUG) $< -o $@ -lsqlite3


.PHONY:clean
clean:
	-rm -f $(OBJS) core *.o
