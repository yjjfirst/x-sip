CC=gcc
CXX=g++

CPPUTEST_HOME=../cpputest
CPPFLAGS += -I$(CPPUTEST_HOME)/include -I../include
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

TESTSUITE_OBJS=AllTests.o FirstTest.o RequestLineTest.o
PRODUCT_OBJS= ../src/RequestLine.o
%.o:%.cpp
	$(CXX) -c -o $@ $< $(CPPFLAGS)
CFLAGS= -I.
AllTests: $(TESTSUITE_OBJS) $(PRODUCT_OBJS)
	$(CXX) -o $@ $^ $(CPPFLAGS) $(LD_LIBRARIES)

clean:
	rm *.o
	rm AllTests
