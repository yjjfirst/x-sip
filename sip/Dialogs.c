#include "Dialogs.h"
#include "CallIdHeader.h"
#include "Utils.h"

#define TAG_MAX_LENGTH 64
struct Dialogs {
};

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

struct Dialog *GetDialogByCallId(struct Dialogs *dialogs, char *callid, char *localTag, char *remoteTag)
{
    return NULL;
}

void AddDialog(struct Dialogs *dialogs, struct Dialog *dialog)
{

}

struct Dialogs *CreateDialogs()
{
    struct Dialogs *dialogs;
    dialogs = calloc(1, sizeof (struct Dialogs));

    return dialogs;
}

void DestoryDialogs(struct Dialogs **dialogs)
{
    if (*dialogs != NULL) {
        free(*dialogs);
    }
}


