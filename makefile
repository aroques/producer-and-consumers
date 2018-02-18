CC = gcc
CFLAGS = -I. -g -Wall

EXEC1 = master
OBJS1 = master.o
DEPS1 =

EXEC2 = producer
OBJS2 = producer.o
DEPS2 =

EXEC3 = consumer
OBJS3 = consumer.o
DEPS3 =

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
	rm $(EXEC1) $(OBJS1) $(EXEC2) $(OBJS2) $(EXEC3) $(OBJS3) 
