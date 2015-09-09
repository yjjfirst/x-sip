struct CallIDHeader ;

struct CallIDHeader *CreateCallIDHeader();
void DestoryCallIDHeader(struct Header *id);
struct ParsePattern *GetCallIDPattern();
struct Header *ParseCallIDHeader(char *string);

char *CallIDHeaderGetID(struct CallIDHeader *id);
char *CallIDHeaderGetName(struct CallIDHeader *id);
