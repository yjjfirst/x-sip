.PHONY: all clean test gui cli

DEBUG_LOG=yes

default: test

cli:
	-@make -C utils all
	-@make -C sip all
	-@make -C cli 
gui:
	-@make -C utils all
	-@make -C sip all
	-@make -C gui 

test: DEBUG_LOG=no;
test:
	-@make -C utils all
	-@make -C sip all
	-@make -C test all
clean:
	-@make -C test clean
	-@make -C sip clean
	-@make -C utils clean
	-@make -C gui clean
	-@make -C cli clean
export TEST=$(MAKECMDGOALS)
export LOG=$(DEBUG_LOG)
