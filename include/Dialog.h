struct DialogId;
struct Dialog;

struct DialogId *DialogGetId(struct Dialog *dialog);

struct Dialog *CreateDialog(struct DialogId *dialogid);
void DestoryDialog(struct Dialog **dialog);
