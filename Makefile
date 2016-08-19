.PHONY: all clean test gui

gui:
	-@make -C utils all
	-@make -C sip all
	-@make -C gui 
test:
	-@make -C utils all
	-@make -C sip all
	-@make -C test all
clean:
	-@make -C test clean
	-@make -C sip clean
	-@make -C utils clean
	-@make -C gui clean

export TEST=$(MAKECMDGOALS)
