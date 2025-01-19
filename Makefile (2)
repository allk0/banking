CC = gcc
CFLAGS = -Wall -g
ZAD1 = zadanie1
ZAD2 = zadanie2

all: $(ZAD1) $(ZAD2)

$(ZAD2): zadanie2.o
	$(CC) $(CFLAGS) -o $(ZAD2) zadanie2.o

$(ZAD1): zadanie1.o
	$(CC) $(CFLAGS) -o $(ZAD1) zadanie1.o

%.o: %.c
	$(CC) $(CFLAGS) -c *.c

clean:
	rm -f *.o $(TARGET)
