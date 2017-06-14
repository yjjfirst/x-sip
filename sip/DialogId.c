#include <stdio.h>

#include "DialogId.h"
#include "CallIdHeader.h"
#include "Messages.h"
#include "Utils.h"
#include "StringExt.h"

#define TAG_MAX_LENGTH 64

struct DialogId {
    char callid[CALLID_MAX_LENGTH];
    char localTag[TAG_MAX_LENGTH];
    char remoteTag[TAG_MAX_LENGTH];
};

DEFINE_STRING_MEMBER_WRITER(struct DialogId, SetCallId, callid, CALLID_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct DialogId, GetCallId, callid);

DEFINE_STRING_MEMBER_WRITER(struct DialogId, SetLocalTag, localTag, TAG_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct DialogId, GetLocalTag, localTag);

DEFINE_STRING_MEMBER_WRITER(struct DialogId, SetRemoteTag, remoteTag, TAG_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct DialogId, GetRemoteTag, remoteTag);

BOOL DialogIdMatched(struct DialogId *id1, struct DialogId *id2)
{
    assert(id1 != NULL);
    assert(id2 != NULL);

    return !(StrcmpExt(id1->callid, id2->callid)
             || StrcmpExt(id1->localTag, id2->localTag)
             || StrcmpExt(id1->remoteTag, id2->remoteTag));
}

struct DialogId *CreateEmptyDialogId()
{
    struct DialogId *dialogid = calloc(1, sizeof (struct DialogId));
    return dialogid;
}

struct DialogId *CreateFixedDialogId(char *callid, char *localTag, char *remoteTag)
{
    struct DialogId *dialogid = CreateEmptyDialogId();
    SetCallId(dialogid, callid);
    SetLocalTag(dialogid, localTag);
    SetRemoteTag(dialogid, remoteTag);

    return dialogid;
}

void ExtractDialogId(struct DialogId *dialogid, MESSAGE *message)
{
    SetCallId(dialogid, MessageGetCallId(message));
    if (MessageType(message) == MESSAGE_TYPE_RESPONSE) {
        SetLocalTag(dialogid, MessageGetFromTag(message));
        SetRemoteTag(dialogid, MessageGetToTag(message));
    } else {
        SetRemoteTag(dialogid, MessageGetFromTag(message));
    }
}

struct DialogId *CreateDialogIdFromMessage(MESSAGE *message)
{
    struct DialogId *dialogid = CreateEmptyDialogId();
    ExtractDialogId(dialogid, message);
    return dialogid;
}

void DestroyDialogId(struct DialogId **id)
{
    if(*id != NULL)
        free(*id);
}
