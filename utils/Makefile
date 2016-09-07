.PYONY: all clean

SRCS = $(wildcard ./*.c)
OBJS = $(SRCS:.c=.o)

%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS) -I../include/utils/

all: $(OBJS)
	make -C list all
	ar rcs libutils.a list/src/*.o *.o
clean:
	make -C list clean
	-@rm -f *.d
	-@rm -f *.o
	-@rm -f *.a
-include $(SRCS:%.c=%.d)
-include ../make.common
