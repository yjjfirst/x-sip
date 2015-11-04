#include "Method.h"
#include "Bool.h"

struct CSeqHeader;

struct HeaderPattern *GetCSeqHeaderPattern();
struct CSeqHeader *CreateCSeqHeader(int seq, char *method);
void DestoryCSeqHeader(struct Header *c);
struct Header *ParseCSeqHeader(char *string);

char *CSeqHeaderGetName(struct CSeqHeader *c);
int CSeqHeaderGetSeq(struct CSeqHeader *c);
char *CSeqHeaderGetMethod(struct CSeqHeader *c);

char *CSeq2String(char *result, struct Header *c);
BOOL CSeqHeaderMethodMatched(struct CSeqHeader *c1, struct CSeqHeader *c2);
