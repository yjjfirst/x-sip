#include <stdio.h>

#include "DialogId.h"
#include "CallIdHeader.h"
#include "Messages.h"
#include "Utils.h"


#define TAG_MAX_LENGTH 64

struct DialogId {
    char callid[CALLID_MAX_LENGTH];
    char localTag[TAG_MAX_LENGTH];
    char remoteTag[TAG_MAX_LENGTH];
};

DEFINE_STRING_MEMBER_WRITER(struct DialogId, DialogIdSetCallId, callid, CALLID_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct DialogId, DialogIdGetCallId, callid);

DEFINE_STRING_MEMBER_WRITER(struct DialogId, DialogIdSetLocalTag, localTag, TAG_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct DialogId, DialogIdGetLocalTag, localTag);

DEFINE_STRING_MEMBER_WRITER(struct DialogId, DialogIdSetRemoteTag, remoteTag, TAG_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct DialogId, DialogIdGetRemoteTag, remoteTag);

BOOL DialogIdMatched(struct DialogId *id1, struct DialogId *id2)
{
    assert(id1 != NULL);
    assert(id2 != NULL);

    return !(strcmp(id1->callid, id2->callid)
             || strcmp(id1->localTag, id2->localTag)
             || strcmp(id1->remoteTag, id2->remoteTag));
}

struct DialogId *CreateEmptyDialogId()
{
    struct DialogId *dialogid = calloc(1, sizeof (struct DialogId));
    return dialogid;
}

struct DialogId *CreateDialogId(char *callid, char *localTag, char *remoteTag)
{
    struct DialogId *dialogid = CreateEmptyDialogId();
    DialogIdSetCallId(dialogid, callid);
    DialogIdSetLocalTag(dialogid, localTag);
    DialogIdSetRemoteTag(dialogid, remoteTag);

    return dialogid;
}

void DialogIdExtractFromMessage(struct DialogId *dialogid, struct Message *message)
{
    DialogIdSetCallId(dialogid, MessageGetCallId(message));
    DialogIdSetLocalTag(dialogid, MessageGetLocalTag(message));
    DialogIdSetRemoteTag(dialogid, MessageGetRemoteTag(message));
}

struct DialogId *CreateDialogIdFromMessage(struct Message *message)
{
    struct DialogId *dialogid = CreateEmptyDialogId();
    DialogIdSetCallId(dialogid, MessageGetCallId(message));
    DialogIdSetLocalTag(dialogid, MessageGetLocalTag(message));
    DialogIdSetRemoteTag(dialogid, MessageGetRemoteTag(message));

    return dialogid;
}

void DestoryDialogId(struct DialogId **id)
{
    if(*id != NULL)
        free(*id);
}
