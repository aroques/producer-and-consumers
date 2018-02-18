
CC = gcc
CFLAGS = -I. -g -Wall

EXEC = master
OBJS = master.o
DEPS =

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: master

mater: $(OBJS)
	gcc -o $(EXEC) $^ $(CFLAGS)

clean:
	rm $(EXEC) $(OBJS)