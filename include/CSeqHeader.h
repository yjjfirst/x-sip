#include "Bool.h"

struct CSeqHeader;

struct HeaderPattern *GetCSeqHeaderPattern();
struct CSeqHeader *CreateCSeqHeader(int seq, char *method);
void DestoryCSeqHeader(struct Header *c);
struct Header *ParseCSeqHeader(char *string);

char *CSeqHeaderGetName(struct CSeqHeader *c);
unsigned int CSeqHeaderGetSeq(struct CSeqHeader *c);
char *CSeqHeaderGetMethod(struct CSeqHeader *c);

void CSeqHeaderSetMethod(struct CSeqHeader *c, char *method);
char *CSeq2String(char *result, struct Header *c);
BOOL CSeqHeaderMethodMatched(struct CSeqHeader *c1, struct CSeqHeader *c2);
BOOL CSeqMethodMatchedByString(struct CSeqHeader *c, char *string);
 
extern unsigned int (*CSeqGenerateSeq)();
