struct MaxForwardsHeader;

struct ParsePattern *GetMaxForwardsPattern();
struct MaxForwardsHeader *CreateMaxForwardsHeader();
void DestoryMaxForwardsHeader(struct Header *maxForward);
struct Header *ParseMaxForwardsHeader(char *string);

char *MaxForwardsGetName(struct MaxForwardsHeader *m);
int MaxForwardsGetMaxForwards(struct MaxForwardsHeader *m);
