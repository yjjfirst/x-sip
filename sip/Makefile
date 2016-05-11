CC=gcc
CXX=g++

CPPFLAGS += -I../include -I.. -I../include/utils  -Wall -g
CPPFLAGS += -include ../cpputest/include/CppUTest/MemoryLeakDetectorMallocMacros.h

.PYONY: all clean

SRCS = $(wildcard ./*.c)
OBJS = $(SRCS:.c=.o)

%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS)

libsip.a: ../utils/libutils.a sip.a
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
