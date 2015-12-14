#include "Dialogs.h"
#include "CallIdHeader.h"

#define TAG_MAX_LENGTH 64

struct Dialogs {
};

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


