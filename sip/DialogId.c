#include "DialogId.h"
#include "CallIdHeader.h"
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
    return !(strcmp(id1->callid, id2->callid)
             || strcmp(id1->localTag, id2->localTag)
             || strcmp(id1->remoteTag, id2->remoteTag));
}

struct DialogId *CreateDialogId(char *callid, char *localTag, char *remoteTag)
{
    struct DialogId *dialogid = calloc(1, sizeof (struct DialogId));
    DialogIdSetCallId(dialogid, callid);
    DialogIdSetLocalTag(dialogid, localTag);
    DialogIdSetRemoteTag(dialogid, remoteTag);

    return dialogid;
}

void DestoryDialogId(struct DialogId **id)
{
    if(*id != NULL)
        free(*id);
}
