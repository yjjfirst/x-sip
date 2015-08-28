#include "Parser.h"

struct CallIDHeader ;

struct CallIDHeader *CreateCallIDHeader();
void DestoryCallIDHeader(struct CallIDHeader *id);
struct ParsePattern *GetCallIDPattern();

char *CallIDHeaderGetID(struct CallIDHeader *id);
char *CallIDHeaderGetName(struct CallIDHeader *id);
