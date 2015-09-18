struct CallIDHeader ;

struct CallIDHeader *CreateCallIDHeader();
void DestoryCallIDHeader(struct Header *id);
struct HeaderPattern *GetCallIDPattern();
struct Header *ParseCallIDHeader(char *string);

char *CallIDHeaderGetID(struct CallIDHeader *id);
char *CallIDHeaderGetName(struct CallIDHeader *id);

char *CallIDHeader2String(char *result, struct Header *id);
