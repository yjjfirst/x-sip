#include "Parser.h"
struct MaxForwardsHeader;

struct ParsePattern *GetMaxForwardPattern();
struct MaxForwardsHeader *CreateMaxForwardsHeader();
void DestoryMaxForwardsHeader(struct MaxForwardsHeader *maxForward);

char *MaxForwardsGetName(struct MaxForwardsHeader *m);
int MaxForwardsGetMaxForwards(struct MaxForwardsHeader *m);
