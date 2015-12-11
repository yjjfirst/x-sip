struct Dialogs;
struct DialogId;
struct CallIdHeader;

struct Dialog *GetDialogByCallId(struct Dialogs *dialogs, char *callid, char *localTag, char *remoteTag);
struct Dialogs *CreateDialogs();
struct DialogId *CreateDialogId(char *callid, char *localTag, char *remoteTag);
void DestoryDialogs(struct Dialogs **dialogs);
void AddDialog(struct Dialogs *dialogs, struct Dialog *dialog);

void DestoryDialogId(struct DialogId **id);
char *DialogIdGetCallId(struct DialogId *dialid);
char *DialogIdGetLocalTag(struct DialogId *dialid);
char *DialogIdGetRemoteTag(struct DialogId *dialid);
