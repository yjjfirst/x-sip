.PHONY: all clean

all:
	-@make -C utils all
	-@make -C sip all
	-@make -C test all
	-@make -C gui 

clean:
	-@make -C test clean
	-@make -C sip clean
	-@make -C utils clean
	-@make -C gui clean

