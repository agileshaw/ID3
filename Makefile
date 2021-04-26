CC = gcc
CFLAGS = -Wall -g

all: classifier
classifier: main.o data.o rules.o train.o node.o
	$(CC) $(CFLAGS) -o classifier main.o data.o rules.o train.o node.o -lm
main.o: main.c main.h
	$(CC) $(CFLAGS) -c main.c -lm
data.o: data.c data.h
	$(CC) $(CFLAGS) -c data.c -lm
rules.o: rules.c rules.h
	$(CC) $(CFLAGS) -c rules.c -lm
train.o: train.c train.h
	$(CC) $(CFLAGS) -c train.c -lm
node.o: node.c node.h
	$(CC) $(CFLAGS) -c node.c -lm

clean:
	rm *.o
.PHONY:
	classifier
