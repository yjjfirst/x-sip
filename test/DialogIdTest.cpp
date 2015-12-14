#include "CppUTest/TestHarness.h"

TEST_GROUP(DialogIdTestGroup)
{
};

extern "C" {
#include "DialogId.h"
}

TEST(DialogIdTestGroup, CreateDialogIdTest)
{
    char callid[] = "1";
    char localTag[] = "2";
    char remoteTag[] = "3";

    struct DialogId *dialogId = CreateDialogId(callid, localTag, remoteTag);
    STRCMP_EQUAL("1", DialogIdGetCallId(dialogId));
    STRCMP_EQUAL("2", DialogIdGetLocalTag(dialogId));
    STRCMP_EQUAL("3", DialogIdGetRemoteTag(dialogId));

    DestoryDialogId(&dialogId);
}

TEST(DialogIdTestGroup, DialogIdEqualTest)
{
    struct DialogId *id1 = CreateDialogId((char *)"1", (char *)"2", (char *)"3");
    struct DialogId *id2 = CreateDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_TRUE(DialogIdMatched(id1, id2));

    DestoryDialogId(&id1);
    DestoryDialogId(&id2);
}

TEST(DialogIdTestGroup, DialogIdUnequalTest)
{
    struct DialogId *id1 = CreateDialogId((char *)"2", (char *)"2", (char *)"3");
    struct DialogId *id2 = CreateDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_FALSE(DialogIdMatched(id1, id2));

    DestoryDialogId(&id1);
    DestoryDialogId(&id2);
}

TEST(DialogIdTestGroup, DialogIdUnequalTest1)
{
    struct DialogId *id1 = CreateDialogId((char *)"4", (char *)"5", (char *)"6");
    struct DialogId *id2 = CreateDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_FALSE(DialogIdMatched(id1, id2));

    DestoryDialogId(&id1);
    DestoryDialogId(&id2);
}

TEST(DialogIdTestGroup, DialogIdUnequalTest2)
{
    struct DialogId *id1 = CreateDialogId((char *)"1", (char *)"1", (char *)"6");
    struct DialogId *id2 = CreateDialogId((char *)"1", (char *)"2", (char *)"3");

    CHECK_FALSE(DialogIdMatched(id1, id2));

    DestoryDialogId(&id1);
    DestoryDialogId(&id2);
}
