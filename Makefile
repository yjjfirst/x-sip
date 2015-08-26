.PHONY: all clean

all:
	-@make -C utils all
	-@make -C src all
	-@make -C test all

clean:
	-@make -C test clean
	-@make -C src clean
	-@make -C utils clean

