CC=gcc
CXX=g++

CPPUTEST_HOME=../cpputest
CPPFLAGS += -I$(CPPUTEST_HOME)/include -I../include
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt
TEST_SRCS = AllTests.cpp FirstTest.cpp RequestLineTest.cpp URITtest.cpp 
PRODUCT_SRCS = ../src/RequestLine.c ../src/URI.c
CFLAGS= -I.

.PHONY: all

all:AllTests

TESTSUITE_OBJS=AllTests.o FirstTest.o RequestLineTest.o URITest.o ../src/RequestLine.o \
	       	../src/URI.o

%.o:%.cpp
	$(CXX) -MMD -c -o $@ $< $(CPPFLAGS)
%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS)

AllTests: $(TESTSUITE_OBJS) 
	$(CXX) -o $@ $^ $(CPPFLAGS) $(LD_LIBRARIES)

clean:
	make -C ../src
	@rm *.d
	@rm *.o
	@rm AllTests

-include $(TEST_SRCS:%.cpp=%.d)
-include $(PRODUCT_SRCS:%.c=%.d)
