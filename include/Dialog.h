struct DialogId;
struct Dialog;

struct DialogId *DialogGetId(struct Dialog *dialog);
struct UserAgent *DialogGetUserAgent(struct Dialog *dialog);

struct Dialog *CreateDialog(struct DialogId *dialogid, struct UserAgent *ua);
void DestoryDialog(struct Dialog **dialog);
