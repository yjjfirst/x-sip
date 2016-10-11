#include "Bool.h"
struct Message;
struct DialogId *CreateEmptyDialogId();
struct DialogId *CreateFixedDialogId(char *callid, char *localTag, char *remoteTag);
struct DialogId *CreateDialogIdFromMessage(struct Message *message);
void DestroyDialogId(struct DialogId **id);

char *GetCallId(struct DialogId *dialid);
void SetCallId(struct DialogId *dialid, char *callId);
char *GetLocalTag(struct DialogId *dialid);
void SetLocalTag(struct DialogId *dialid, char *tag);
char *GetRemoteTag(struct DialogId *dialid);
void SetRemoteTag(struct DialogId *dialid, char *tag);
BOOL DialogIdMatched(struct DialogId *id1, struct DialogId *id2);
void ExtractDialogId(struct DialogId *id, struct Message *message);
