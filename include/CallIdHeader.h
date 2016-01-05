#define CALLID_MAX_LENGTH 256

struct CallIdHeader ;
struct Header;

struct CallIdHeader *CreateCallIdHeader(char *idString);
struct CallIdHeader *CreateEmptyCallIdHeader();
void DestoryCallIdHeader(struct Header *id);
struct HeaderPattern *GetCallIdPattern();
struct Header *ParseCallIdHeader(char *string);

char *CallIdHeaderGetID(struct CallIdHeader *id);
char *CallIdHeaderGetName(struct CallIdHeader *id);

char *CallIdHeader2String(char *result, struct Header *id);
char *GenerateCallIdString();
