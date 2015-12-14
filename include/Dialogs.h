struct Dialogs;
struct DialogId;
struct CallIdHeader;

struct Dialog *GetDialogByCallId(struct Dialogs *dialogs, char *callid, char *localTag, char *remoteTag);
struct Dialogs *CreateDialogs();
void DestoryDialogs(struct Dialogs **dialogs);
void AddDialog(struct Dialogs *dialogs, struct Dialog *dialog);
