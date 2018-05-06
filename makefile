CC = gcc
CFLAGS = -I$(IDIR) -g -Wall 
IDIR =./include

EXEC1 = master
OBJS1 = master.o shared_memory.o helpers.o

EXEC2 = producer
OBJS2 = producer.o shared_memory.o helpers.o

EXEC3 = consumer
OBJS3 = consumer.o shared_memory.o helpers.o

DEPS = global_constants.h shared_memory.h helpers.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: master producer consumer

master: $(OBJS1)
	gcc -o $(EXEC1) $^ $(CFLAGS)
	
producer: $(OBJS2)
	gcc -o $(EXEC2) $^ $(CFLAGS)

consumer: $(OBJS3)
	gcc -o $(EXEC3) $^ $(CFLAGS)

clean:
	rm $(EXEC1) $(EXEC2) $(EXEC3) *.o
