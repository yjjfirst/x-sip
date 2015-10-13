struct CallIdHeader ;

struct CallIdHeader *CreateCallIdHeader();
void DestoryCallIdHeader(struct Header *id);
struct HeaderPattern *GetCallIdPattern();
struct Header *ParseCallIdHeader(char *string);

char *CallIdHeaderGetID(struct CallIdHeader *id);
char *CallIdHeaderGetName(struct CallIdHeader *id);

char *CallIdHeader2String(char *result, struct Header *id);
