struct DialogManager;
struct DialogId;
struct CallIdHeader;
struct UserAgent;
struct Message;

void ClearDialogManager();
void DestroyDialogList();
struct DialogManager *CreateDialogs();
void DestroyDialogs(struct DialogManager **dialogs);

struct Dialog *GetDialogById(struct DialogId *dialogid);
struct Dialog *GetDialogByUserAgent(struct UserAgent *ua);
struct Dialog *AddDialog(struct DialogId *dialogid, struct UserAgent *ua);
struct Dialog *AddConfirmedDialog(struct DialogId *dialogid, struct UserAgent *ua);
struct Dialog *MatchMessage2Dialog(struct Message *bye);
void RemoveDialogById(struct DialogId *dialogId);
struct Dialog *GetDialog(int pos);
int CountDialogs();

void DumpDialogManager();
