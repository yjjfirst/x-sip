#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TransportMock.h"
#include "TestingMessages.h"

extern "C" {
#include <stdio.h>
#include "Messages.h"
#include "MessageTransport.h"
#include "TransactionManager.h"
}

TEST_GROUP(IncomingInviteTransactionTestGroup)
{
    void setup(){
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);
        UT_PTR_SET(Transporter, &MockTransporter);
   }

    void teardown() {
        mock().clear();
    }
};

TEST(IncomingInviteTransactionTestGroup, ReceiveInvitedTest)
{
    char stringReceived[MAX_MESSAGE_LENGTH] = {0};

    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INCOMMING_INVITE_MESSAGE);
    ReceiveInMessage(stringReceived);    
    
    EmptyTransactionManager();
}









