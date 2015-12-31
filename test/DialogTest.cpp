#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"
#include "TransportMock.h"
#include "UserAgentTest.h"

extern "C" {
#include <stdio.h>

#include "Transaction.h"
#include "UserAgent.h"
#include "Dialog.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "TransactionManager.h"
#include "MessageTransport.h"
}

TEST_GROUP(DialogTestGroup)
{
    struct UserAgent *ua;
    struct Dialog *dialog;
    void setup()
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(ReceiveMessageCallback, MessageReceived);

        ua = BuildUserAgent();
        dialog = CreateDialog(NULL, ua);
    }

    void teardown()
    {
        EmptyTransactionManager();
        DestoryUserAgent(&ua);
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(DialogTestGroup, AckRequestInviteSuccessedTest)
{
    char revMessage[MAX_MESSAGE_LENGTH] = {0};
    struct Message *message = BuildInviteMessage(dialog);

    mock().expectOneCall("SendOutMessageMock");
    mock().expectOneCall("ReceiveInMessageMock").andReturnValue(INVITE_200OK_MESSAGE);    
   
    AddTransaction(message, (struct TransactionOwner *)dialog);
    UT_PTR_SET(Transporter, &MockTransporterForAck);
    mock().expectOneCall("SendOutMessageMock").withParameter("RemoteTag", "as6151ad25");

    ReceiveInMessage(revMessage);
}
