struct MaxForwardsHeader;

struct ParsePattern *GetMaxForwardsPattern();
struct MaxForwardsHeader *CreateMaxForwardsHeader();
void DestoryMaxForwardsHeader(struct MaxForwardsHeader *maxForward);

char *MaxForwardsGetName(struct MaxForwardsHeader *m);
int MaxForwardsGetMaxForwards(struct MaxForwardsHeader *m);
