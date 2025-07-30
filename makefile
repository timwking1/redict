CC = gcc
CFLAGS =
SRCS = redict.c
OBJS =  redict.o
TARGET = redict.exe

all: $(TARGET)

redict.o: redict.c
	$(CC) -c redict.c $(CFLAGS)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET)

clean:
	rm -f *.o *.exe
	