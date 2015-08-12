CC=gcc
CXX=g++

CPPUTEST_HOME=../cpputest
CPPFLAGS += -I$(CPPUTEST_HOME)/include -I../include
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt
SRCS= AllTests.cpp FirstTest.cpp RequestLineTest.c URITtest.cpp ../src/RequestLine.cpp \
      	../src/URI.cpp
CFLAGS= -I.

.PHONY: all

all:AllTests

TESTSUITE_OBJS=AllTests.o FirstTest.o RequestLineTest.o URITest.o ../src/RequestLine.o \
	       	../src/URI.o

%.o:%.cpp
	$(CXX) -MMD -c -o $@ $< $(CPPFLAGS)

AllTests: $(TESTSUITE_OBJS) 
	$(CXX) -o $@ $^ $(CPPFLAGS) $(LD_LIBRARIES)

clean:
	@rm *.d
	@rm *.o
	@rm AllTests

-include $(SRCS:%.cpp=%.d)
