#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "TestingMessages.h"
#include "TransportMock.h"
#include "UserAgentTest.h"

extern "C" {
#include "Transaction.h"
#include "UserAgent.h"
#include "Dialog.h"
#include "MessageBuilder.h"
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
        DestoryUserAgent(&ua);
        mock().clear();
    }
};

TEST(DialogTestGroup, AckSendAfterInviteSuccessedTest)
{
    // struct Message *message = BuildInviteMessage(dialog);

    // mock().expectOneCall("SendMessageMock");
    // AddTransaction(message, (struct TransactionOwner *)dialog);
}
