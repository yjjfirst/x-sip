CC=gcc
CXX=g++

CPPFLAGS += -I../include -I..  -Wall -g
CPPFLAGS += -include ../cpputest/include/CppUTest/MemoryLeakDetectorMallocMacros.h


.PYONY: all clean

OBJS = Parser.o RequestLine.o URI.o ContactHeader.o ViaHeader.o MaxForwardsHeader.o \
       CallIdHeader.o ContentLengthHeader.o ExpiresHeader.o Parameter.o\
       StatusLine.o CSeqHeader.o Messages.o MessageBuilder.o MessageTransport.o

SRCS = $(OBJS:%o=%c)
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
