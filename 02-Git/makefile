
CC=gcc

INCLUDES=

LIBS=

DEFINEDS=

CFLAGS=$(INCLUDES) $(LIBS) -O0 -ggdb3

all:
	$(CC) $(CFLAGS) $(DEFINEDS) -c main.c -lX11 -lGL -ldl -o main.o
	$(CC) $(CFLAGS) $(DEFINEDS) main.o -o simple_program
	chmod +x simple_program

clean:
	rm -f main.o
	rm -f simple_program