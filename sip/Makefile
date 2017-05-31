CPPFLAGS += -I../include -I.. -I../include/utils  -Wall -g

ifeq ($(strip $(TEST)), test)
CPPFLAGS += -include ../cpputest/include/CppUTest/MemoryLeakDetectorMallocMacros.h
else
CPPFLAGS += -include stdlib.h
endif

ifeq ($(strip $(LOG)), yes)
CPPFLAGS += -DBUILD_LOG
endif

.PYONY: all clean

SRCS = $(wildcard ./*.c)
OBJS = $(SRCS:.c=.o)

%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS)

libsip.a: ../utils/libutils.a sip.a
	ar -M <libsip.mri

sip.a: $(OBJS)
	ar rcs $@ $^
all: libsip.a

clean:
	-@rm -f *.d
	-@rm -f *.o
	-@rm -f *.a
-include $(SRCS:%.c=%.d)
-include ../make.common
