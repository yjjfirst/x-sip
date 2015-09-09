struct CSeqHeader;

struct ParsePattern *GetCSeqParsePattern();
struct CSeqHeader *CreateCSeqHeader();
void DestoryCSeqHeader(struct Header *c);
struct Header *ParseCSeqHeader(char *string);

char *CSeqHeaderGetName(struct CSeqHeader *c);
int CSeqHeaderGetSeq(struct CSeqHeader *c);
char *CSeqHeaderGetMethod(struct CSeqHeader *c);
