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
#include "DialogManager.h"
}

TEST_GROUP(UserAgentTestGroup)
{    
    struct UserAgent *ua;
    struct Dialog *dialog;
    MESSAGE *message;

    void setup()
    {
        UT_PTR_SET(SipTransporter, &MockTransporter);
        UT_PTR_SET(GenerateBranch, GenerateBranchMock);

        AccountInit();
    }

    void teardown()
    {
        ClearAccountManager();
        mock().checkExpectations();
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
    dialog = AddDialog(dialogid, ua);

    CHECK_TRUE(GetDialogById(dialogid) != NULL);
    POINTERS_EQUAL(dialog, GetDialogById(dialogid));

    DestroyUserAgent(&ua);
}

TEST(UserAgentTestGroup, CountDialogTest)
{
    ClearUaManager();
    ua = AddUa(0);
    dialog = AddDialog(NULL, ua);

    CHECK_EQUAL(1, CountDialogs());

    ClearUaManager();
    
}

TEST(UserAgentTestGroup, GetDialogTest)
{
    ClearUaManager();
    ua = AddUa(0);
    dialog = AddDialog(NULL_DIALOG_ID, ua);
    
    POINTERS_EQUAL(dialog, GetDialog(0));
    ClearUaManager();
}

void DialogRingingMock(struct Dialog *dialog)
{
    mock().actualCall("DialogTerminate").withParameter("dialog", dialog);
}

TEST(UserAgentTestGroup, RingingTest)
{
    UT_PTR_SET(DialogRinging, DialogRingingMock);
    ua = AddUa(0);
    dialog = AddDialog(NULL, ua);

    mock().expectOneCall("DialogTerminate").withParameter("dialog", dialog);
    UaRinging(ua);
    ClearUaManager();
}
