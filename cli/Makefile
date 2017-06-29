.PHONY: all test

CPPUTEST_HOME = ../cpputest

CPPFLAGS += -I$(CPPUTEST_HOME)/include
CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

all: AllTests cli
test: AllTests

TESTSUITE_SRCS = $(wildcard ./*.cpp)
TESTSUITE_OBJS = $(TESTSUITE_SRCS:.cpp=.o)
TESTSUITE_OBJS += cli_command.o

CPPFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

%.o:%.cpp
	$(CXX) -MMD -c -o $@ $< $(CPPFLAGS)
%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS)

AllTests: $(TESTSUITE_OBJS) ../sip/libsip.a ../utils/libutils.a
	$(CXX) -o $@ $^ $(CPPFLAGS) $(LD_LIBRARIES) 


cli : cli.c ../sip/libsip.a ../utils/libutils.a
	$(CC)  -o cli  cli.c  ../sip/libsip.a ../utils/libutils.a -I../include
clean :
	-@rm -f cli
	-@rm -f *.o
	-@rm -f *.d
	-@rm -f AllTests
