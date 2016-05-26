CC=gcc
CXX=g++

CPPUTEST_HOME=../cpputest
CPPFLAGS += -I./ -I../ -I$(CPPUTEST_HOME)/include -I../include -I../include/utils -Wall -g
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

.PHONY: all

all:AllTests

TESTSUITE_SRCS = $(wildcard ./*.cpp)
TESTSUITE_OBJS = $(TESTSUITE_SRCS:.cpp=.o)

CPPFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

%.o:%.cpp
	$(CXX) -MMD -c -o $@ $< $(CPPFLAGS)
%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS)

AllTests: $(TESTSUITE_OBJS) ../sip/libsip.a ../utils/libutils.a
	$(CXX) -o $@ $^ $(CPPFLAGS) $(LD_LIBRARIES) 

clean:
	-@rm -f *.d
	-@rm -f *.o
	-@rm -f AllTests

-include $(TESTSUITE_SRCS:%.cpp=%.d)
-include ../make.common
