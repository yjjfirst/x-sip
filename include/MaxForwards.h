struct MaxForwardsHeader;

struct HeaderPattern *GetMaxForwardsPattern();
struct MaxForwardsHeader *CreateMaxForwardsHeader();
void DestoryMaxForwardsHeader(struct Header *maxForward);
struct Header *ParseMaxForwardsHeader(char *string);

char *MaxForwardsGetName(struct MaxForwardsHeader *m);
int MaxForwardsGetMaxForwards(struct MaxForwardsHeader *m);

void MaxForwards2String(char *result, struct MaxForwardsHeader *m);
