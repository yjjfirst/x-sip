CC=gcc
CXX=g++

CPPUTEST_HOME=../cpputest
CPPFLAGS += -I$(CPPUTEST_HOME)/include -I../include -Wall -g
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt
TEST_SRCS = AllTests.cpp FirstTest.cpp RequestLineTest.cpp URITtest.cpp ContactsHeaderTest.cpp \
	    ViaHeaderTest.cpp MaxForwardsTest.cpp CallIDHeaderTest.cpp CSeqHeaderTest.cpp ExpiresHeaderTest.cpp \
	    ContentLengthHeaderTest.cpp MessagesTest.cpp

.PHONY: all

all:AllTests

TESTSUITE_OBJS=AllTests.o FirstTest.o RequestLineTest.o URITest.o ContactsHeaderTest.o ViaHeaderTest.o MaxForwardsTest.o \
	CallIDHeaderTest.o CSeqHeaderTest.o ExpiresHeaderTest.o ContentLengthHeaderTest.o MessagesTest.o

CPPFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

%.o:%.cpp
	$(CXX) -MMD -c -o $@ $< $(CPPFLAGS)
%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS)

AllTests: $(TESTSUITE_OBJS) ../src/libsip.a 
	$(CXX) -o $@ $^ $(CPPFLAGS) $(LD_LIBRARIES) 

clean:
	-@rm -f *.d
	-@rm -f *.o
	-@rm -f AllTests

-include $(TEST_SRCS:%.cpp=%.d)
-include $(PRODUCT_SRCS:%.c=%.d)
-include ../make.common
