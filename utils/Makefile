CC=gcc
CXX=g++

CPPFLAGS += -I../include -Wall -g
SRCS = list.c 


.PYONY: all clean

OBJS = list.o

%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS)
utils.a: $(OBJS)
	ar rcs $@ $^
all:utils.a

clean:
	-@rm -f *.d
	-@rm -f *.o
	-@rm -f *.a
-include $(SRCS:%.c=%.d)

