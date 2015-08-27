CC=gcc
CXX=g++

CPPFLAGS += -I../include -Wall -g
SRCS = RequestLine.c URI.c Parser.c Contacts.c ViaHeader.c MaxForwards.c


.PYONY: all clean

OBJS = Parser.o RequestLine.o URI.o Contacts.o ViaHeader.o MaxForwards.o

%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS)
sip.a: $(OBJS)
	ar rcs $@ $^
all:sip.a

clean:
	-@rm -f *.d
	-@rm -f *.o
	-@rm -f *.a
-include $(SRCS:%.c=%.d)

