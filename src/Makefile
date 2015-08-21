CC=gcc
CXX=g++

CPPFLAGS += -I../include
SRCS = RequestLine.c URI.c Parser.c ToHeader.c list.c


.PYONY: all clean

OBJS = Parser.o RequestLine.o URI.o ToHeader.o list.o

%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS)
sip.o: $(OBJS)
	$(CC) -c -o $@ $^ $(CPPFLAGS)

all:sip.o

clean:
	rm *.d
	rm *.o
