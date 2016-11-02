#include "CppUTest/TestHarness.h"
#include "TestingMessages.h"

extern "C" {
#include "DialogManager.h"
#include "Dialog.h"
#include "DialogId.h"
#include "UserAgent.h"
#include "UserAgentManager.h"
#include "Messages.h"
}

TEST_GROUP(DialogsTestGroup)
{
};

TEST(DialogsTestGroup, CreateDialogsTest)
{
    struct DialogManager *dialogs = CreateDialogs();
    
    CHECK_TRUE(dialogs != NULL);

    DestroyDialogs(&dialogs);
}

TEST(DialogsTestGroup, AddDialogTest)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct Dialog *dialog = AddDialog(dialogid, ua);
           
    POINTERS_EQUAL(dialog, GetDialogById(dialogid));
    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, RemoveDialogTest)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    AddDialog(dialogid, ua);

    RemoveDialog(dialogid);
    POINTERS_EQUAL(NULL, GetDialogById(dialogid));

    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, RemoveDialogWithWrongId)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct DialogId *dialogid2 = CreateFixedDialogId((char *)"aa",(char *) "bb",(char *)"cc");    
    struct Dialog *dialog = AddDialog(dialogid, ua);

    RemoveDialog(dialogid2);
    POINTERS_EQUAL(dialog, GetDialogById(dialogid));

    DestroyUserAgent(&ua);
    DestroyDialogId(&dialogid2);
}

TEST(DialogsTestGroup, CountDialogsTest)
{
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct DialogId *dialogid2 = CreateFixedDialogId((char *)"aa",(char *) "bb",(char *)"cc");    

    AddDialog(dialogid, ua);
    CHECK_EQUAL(1, CountDialogs());

    AddDialog(dialogid2, ua);
    CHECK_EQUAL(2, CountDialogs());
    
    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, GetDialogTest)
{
    ClearDialogManager();

    struct UserAgent *ua =  CreateUserAgent(0);

    struct Dialog *dialog = AddDialog(NULL_DIALOG_ID, ua);    
    struct Dialog *dialog1 = AddDialog(NULL_DIALOG_ID, ua);

    POINTERS_EQUAL(dialog, GetDialog(0));
    POINTERS_EQUAL(dialog1, GetDialog(1));

    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, GetDialogInFirstUserAgentTest)
{
    struct UserAgent *ua = AddUserAgent(0);
    AddUserAgent(1);
    AddUserAgent(0);
    AddUserAgent(0);
    AddUserAgent(0);

    struct Dialog *d = AddDialog(NULL_DIALOG_ID, ua);

    POINTERS_EQUAL(d, GetDialogByUserAgent(ua));

    ClearUserAgentManager();
}

TEST(DialogsTestGroup, GetDialogInLastUserAgentTest)
{
    AddUserAgent(0);
    AddUserAgent(1);
    AddUserAgent(0);
    AddUserAgent(0);
    struct UserAgent *ua = AddUserAgent(0);

    struct Dialog *d = AddDialog(NULL_DIALOG_ID, ua);

    POINTERS_EQUAL(d, GetDialogByUserAgent(ua));

    ClearUserAgentManager();
}

TEST(DialogsTestGroup, GetDialogInMiddleUserAgentTest)
{
    AddUserAgent(0);
    AddUserAgent(1);
    struct UserAgent *ua = AddUserAgent(0);
    AddUserAgent(0);
    AddUserAgent(0);

    struct Dialog *d = AddDialog(NULL_DIALOG_ID, ua);

    POINTERS_EQUAL(d, GetDialogByUserAgent(ua));

    ClearUserAgentManager();
}

TEST(DialogsTestGroup, ClearDialogMangerTest)
{

    ClearDialogManager();
    
    struct UserAgent *ua =  CreateUserAgent(0);
    struct DialogId *dialogid = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct DialogId *dialogid2 = CreateFixedDialogId((char *)"aa",(char *) "bb",(char *)"cc");    

    AddDialog(dialogid, ua);
    CHECK_EQUAL(1, CountDialogs());

    AddDialog(dialogid2, ua);
    CHECK_EQUAL(2, CountDialogs());

    ClearDialogManager();
    CHECK_EQUAL(0, CountDialogs());

    DestroyUserAgent(&ua);
}

TEST(DialogsTestGroup, RequestMatch2DialogTest)
{
    struct Message *bye = CreateMessage();
    ParseMessage(INCOMMING_BYE_MESSAGE,bye);

    struct UserAgent *ua = AddUserAgent(0);
    struct DialogId *id = CreateFixedDialogId((char *)"1312549293",
                                              (char *)"as317b5f26",
                                              (char *)"2074940689");
    struct DialogId *dialogid1 = CreateFixedDialogId((char *)"a",(char *) "b",(char *)"c");    
    struct DialogId *dialogid2 = CreateFixedDialogId((char *)"aa",(char *) "bb",(char *)"cc");    

    AddConfirmedDialog(dialogid1, ua);
    struct Dialog *dialog = AddConfirmedDialog(id, ua);
    AddConfirmedDialog(dialogid2, ua);

    POINTERS_EQUAL(dialog, MatchMessage2Dialog(bye));

    ClearUserAgentManager();
}

TEST(DialogsTestGroup, AddConfirmedDialogTest)
{
    char localTag[] = "asdf-local-tag";
    char remoteTag[] = "asdf-remote-tag";
    char callid[] = "test-call-id";

    struct UserAgent *ua = AddUserAgent(0);
    struct DialogId *id = CreateFixedDialogId(callid, localTag, remoteTag);
    struct Dialog *dialog = AddConfirmedDialog(id, ua);

    CHECK_EQUAL(GetDialogState(dialog), DIALOG_STATE_CONFIRMED);
    STRCMP_EQUAL(DialogGetLocalTag(dialog), localTag);
    STRCMP_EQUAL(DialogGetRemoteTag(dialog), remoteTag);
    STRCMP_EQUAL(DialogGetCallId(dialog), callid);

    ClearUserAgentManager();
}
