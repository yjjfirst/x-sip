CC=gcc
CXX=g++

CPPFLAGS += -I../include -Wall
SRCS = RequestLine.c URI.c Parser.c ToHeader.c list.c


.PYONY: all clean

OBJS = Parser.o RequestLine.o URI.o ToHeader.o list.o

%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS)
libsip.a: $(OBJS)
	ar rcs $@ $^
all:libsip.a

clean:
	-@rm -f *.d
	-@rm -f *.o
	-@rm -f *.a
-include $(SRCS:%.c=%.d)

