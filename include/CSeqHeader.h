#include "Parser.h"

struct CSeqHeader;

struct ParsePattern *GetCSeqParsePattern();
struct CSeqHeader *CreateCSeqHeader();
void DestoryCSeqHeader(struct CSeqHeader *c);

char *CSeqHeaderGetName(struct CSeqHeader *c);
int CSeqHeaderGetSeq(struct CSeqHeader *c);
char *CSeqHeaderGetMethod(struct CSeqHeader *c);
