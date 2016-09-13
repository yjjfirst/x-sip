#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "Mock.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "AccountManager.h"
#include "UserAgent.h"
#include "Accounts.h"
#include "UserAgentManager.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "TransactionManager.h"
#include "Transaction.h"
#include "Transporter.h"
#include "TestingMessages.h"
#include "DialogId.h"
#include "Dialog.h"
#include "Provision.h"
#include "ViaHeader.h"
}

TEST_GROUP(UserAgentTestGroup)
{    
    struct UserAgent *ua;
    struct Dialog *dialog;
    MESSAGE *message;

    void setup()
    {
        UT_PTR_SET(Transporter, &MockTransporter);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);

        AccountInit();
    }

    void teardown()
    {
        ClearAccountManager();
        mock().clear();
    }    
};

TEST(UserAgentTestGroup, CreateUserAgentTest)
{
    struct UserAgent *ua = CreateUserAgent(0);

    CHECK_TRUE(ua != NULL);
    DestroyUserAgent(&ua);
    CHECK(ua == NULL);
}

TEST(UserAgentTestGroup, AddDialogTest)
{
    ua = CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"1", (char *)"2",(char *)"3");
    dialog = AddNewDialog(dialogid, ua);

    CHECK_TRUE(UserAgentGetDialogById(ua, dialogid) != NULL);
    POINTERS_EQUAL(dialog, UserAgentGetDialogById(ua, dialogid));

    DestroyUserAgent(&ua);
}

TEST(UserAgentTestGroup, CountDialogTest)
{
    ua = AddUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"1", (char *)"2",(char *)"3");
    dialog = AddNewDialog(dialogid, ua);

    CHECK_EQUAL(1, UserAgentCountDialogs(ua));

    ClearUserAgentManager();
    
}

TEST(UserAgentTestGroup, GetDialogManagerTest)
{
    ua = AddUserAgent(0);
    CHECK_FALSE(UserAgentGetDialogManager(ua) == NULL);

    ClearUserAgentManager();
}

TEST(UserAgentTestGroup, GetDialogTest)
{
    ua = AddUserAgent(0);
    dialog = AddNewDialog(NULL_DIALOG_ID, ua);
    
    POINTERS_EQUAL(dialog, UserAgentGetDialog(ua, 0));
    ClearUserAgentManager();
}
