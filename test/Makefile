CC=gcc
CXX=g++

CPPUTEST_HOME=../cpputest
CPPFLAGS += -I./ -I$(CPPUTEST_HOME)/include -I../include -Wall -g
LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest -lCppUTestExt

.PHONY: all

all:AllTests

TESTSUITE_OBJS=AllTests.o FirstTest.o RequestLineTest.o URITest.o ContactsHeaderTest.o ViaHeaderTest.o MaxForwardsTest.o\
	CallIdHeaderTest.o CSeqHeaderTest.o ExpiresHeaderTest.o ContentLengthHeaderTest.o MessagesTest.o \
	StatusLineTest.o MessageBuilderTest.o MessageTransportTest.o SessionTest.o ParameterTest.o NonInviteTransactionTest.o \
	TimerTest.o TransactionManagerTest.o UserAgentTest.o SipMethodTest.o InviteTransactionTest.o DialogTest.o \
	DialogsTest.o

TEST_SRCS = $(TESTSUITE_OBJS:%.o=%.cpp)

CPPFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h

%.o:%.cpp
	$(CXX) -MMD -c -o $@ $< $(CPPFLAGS)
%.o:%.c
	$(CC) -MMD -c -o $@ $< $(CPPFLAGS)

AllTests: $(TESTSUITE_OBJS) ../sip/libsip.a 
	$(CXX) -o $@ $^ $(CPPFLAGS) $(LD_LIBRARIES) 

clean:
	-@rm -f *.d
	-@rm -f *.o
	-@rm -f AllTests

-include $(TEST_SRCS:%.cpp=%.d)
-include $(PRODUCT_SRCS:%.c=%.d)
-include ../make.common
