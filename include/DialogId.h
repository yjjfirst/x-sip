#include "Bool.h"
struct Message;
struct DialogId *CreateEmptyDialogId();
struct DialogId *CreateDialogId(char *callid, char *localTag, char *remoteTag);
struct DialogId *CreateDialogIdFromMessage(struct Message *message);
void DestoryDialogId(struct DialogId **id);

char *DialogIdGetCallId(struct DialogId *dialid);
char *DialogIdGetLocalTag(struct DialogId *dialid);
char *DialogIdGetRemoteTag(struct DialogId *dialid);
BOOL DialogIdMatched(struct DialogId *id1, struct DialogId *id2);
