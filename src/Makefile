CC=gcc
CXX=g++

CPPFLAGS += -I../include -I..  -Wall -g
SRCS = RequestLine.c URI.c Parser.c Contacts.c ViaHeader.c MaxForwards.c CallIDHeader.c ContentLength.c ExpiresHeader.c \
       CSeqHeader.c Messages.c
CPPFLAGS += -include ../cpputest/include/CppUTest/MemoryLeakDetectorMallocMacros.h


.PYONY: all clean

OBJS = Parser.o RequestLine.o URI.o Contacts.o ViaHeader.o MaxForwards.o CallIDHeader.o ContentLength.o ExpiresHeader.o \
       CSeqHeader.o Messages.o

%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS)

libsip.a: ../utils/list/liblist.a sip.a
	ar -M <libsip.mri

sip.a: $(OBJS)
	ar rcs $@ $^
all:libsip.a

clean:
	-@rm -f *.d
	-@rm -f *.o
	-@rm -f *.a
-include $(SRCS:%.c=%.d)
-include ../make.common
