#include "Bool.h"

#define CALLID_MAX_LENGTH 256

struct CallIdHeader ;
struct Header;

struct CallIdHeader *CreateCallIdHeader(char *idString);
struct CallIdHeader *CreateEmptyCallIdHeader();
struct CallIdHeader *CallIdHeaderDup(struct CallIdHeader *src);
void DestroyCallIdHeader(struct Header *id);
struct HeaderPattern *GetCallIdPattern();
struct Header *ParseCallIdHeader(char *string);

char *CallIdHeaderGetId(struct CallIdHeader *id);
char *CallIdHeaderGetName(struct CallIdHeader *id);

BOOL CallIdHeaderMatched(struct CallIdHeader *id1, struct CallIdHeader *id2);
char *CallIdHeader2String(char *result, struct Header *id);
void GenerateCallIdString();
