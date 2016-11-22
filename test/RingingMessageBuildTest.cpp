#include "CppUTest/TestHarness.h"
#include "Mock.h"
#include "TestingMessages.h"

extern "C" {
#include <stdio.h>
#include <string.h>

#include "URI.h"
#include "StatusLine.h"
#include "MessageBuilder.h"
#include "Messages.h"
#include "ViaHeader.h"
#include "UserAgent.h"
#include "Dialog.h"
#include "UserAgentManager.h"
#include "AccountManager.h"
#include "DialogManager.h"
#include "Accounts.h"
}

TEST_GROUP(RingingMessageBuildTestGroup)
{
    struct UserAgent *ua;
    MESSAGE *m;
    struct Dialog *dialog;
    void setup()
    {
        AccountInit();
        ua = AddUa(0);
        dialog = AddDialog(NULL_DIALOG_ID, ua);
    }

    void teardown()
    {
        ClearAccountManager();
        ClearDialogManager();
        ClearUaManager();
        DestroyMessage(&m);
    }
};

TEST(RingingMessageBuildTestGroup, RingingMessageStatusLineTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *ringing = BuildResponse(invite, STATUS_CODE_RINGING);
    
    struct StatusLine *sl = MessageGetStatusLine(ringing);

    CHECK_EQUAL(180, StatusLineGetStatusCode(sl));
    STRCMP_EQUAL("Ringing", StatusLineGetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&ringing);
}

TEST(RingingMessageBuildTestGroup, RingingMessageDestAddrTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    SetMessageAddr(invite, (char *)"192.168.10.62");
    MESSAGE *ringing = BuildResponse(invite, STATUS_CODE_RINGING);
    STRCMP_EQUAL(AccountGetProxyAddr(GetAccount(0)), GetMessageAddr(ringing));
    
    DestroyMessage(&invite);
    DestroyMessage(&ringing);
}

TEST(RingingMessageBuildTestGroup, RingingMessageDestPortTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    SetMessagePort(invite, 5060);
    MESSAGE *ringing = BuildResponse(invite, STATUS_CODE_RINGING);
    CHECK_EQUAL(AccountGetProxyPort(GetAccount(0)), GetMessagePort(ringing));
    
    DestroyMessage(&invite);
    DestroyMessage(&ringing);
}
