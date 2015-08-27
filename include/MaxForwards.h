#include "Parser.h"
struct MaxForwardsHeader;

struct ParsePattern *GetMaxForwardPattern();
struct MaxForwardsHeader *CreateMaxForwardsHeader();
void DestoryMaxForwardsHeader(struct MaxForwardsHeader *maxForward);

char *MaxForwardsGetName(struct MaxForwardsHeader *m);
char *MaxForwardsGetMaxForwards(struct MaxForwardsHeader *m);
