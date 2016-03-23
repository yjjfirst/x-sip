#include "Bool.h"
struct Message;
struct DialogId *CreateEmptyDialogId();
struct DialogId *CreateDialogId(char *callid, char *localTag, char *remoteTag);
struct DialogId *CreateDialogIdFromMessage(struct Message *message);
void DestoryDialogId(struct DialogId **id);

char *DialogIdGetCallId(struct DialogId *dialid);
void DialogIdSetCallId(struct DialogId *dialid, char *callId);
char *DialogIdGetLocalTag(struct DialogId *dialid);
void DialogIdSetLocalTag(struct DialogId *dialid, char *tag);
char *DialogIdGetRemoteTag(struct DialogId *dialid);
void DialogIdSetRemoteTag(struct DialogId *dialid, char *tag);
BOOL DialogIdMatched(struct DialogId *id1, struct DialogId *id2);
void DialogIdExtractFromMessage(struct DialogId *id, struct Message *message);
