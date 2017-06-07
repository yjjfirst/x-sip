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
}

TEST_GROUP(RingingMessageBuildTestGroup)
{
    MESSAGE *m;
    void setup()
    {
    }

    void teardown()
    {
        DestroyMessage(&m);
    }
};

TEST(RingingMessageBuildTestGroup, RingingMessageStatusLineTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);
    MESSAGE *ringing = BuildResponse(invite, STATUS_CODE_RINGING);
    
    struct StatusLine *sl = MessageGetStatusLine(ringing);

    CHECK_EQUAL(180, GetStatusCode(sl));
    STRCMP_EQUAL("Ringing", GetReasonPhrase(sl));

    DestroyMessage(&invite);
    DestroyMessage(&ringing);
}

TEST(RingingMessageBuildTestGroup, RingingMessageDestAddrTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    SetMessageAddr(invite, (char *)"192.168.10.62");
    MESSAGE *ringing = BuildResponse(invite, STATUS_CODE_RINGING);
    STRCMP_EQUAL((char *)"192.168.10.62", GetMessageAddr(ringing));
    
    DestroyMessage(&invite);
    DestroyMessage(&ringing);
}

TEST(RingingMessageBuildTestGroup, RingingMessageDestPortTest)
{
    MESSAGE *invite = CreateMessage();
    ParseMessage(INCOMMING_INVITE_MESSAGE, invite);

    SetMessagePort(invite, 5060);
    MESSAGE *ringing = BuildResponse(invite, STATUS_CODE_RINGING);
    CHECK_EQUAL(5060, GetMessagePort(ringing));
    
    DestroyMessage(&invite);
    DestroyMessage(&ringing);
}
