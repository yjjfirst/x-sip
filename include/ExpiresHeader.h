#include "Parser.h"

struct ExpiresHeader;

struct ExpiresHeader *CreateExpiresHeader();
void DestoryExpiresHeader(struct ExpiresHeader *e);
struct ParsePattern *GetExpiresPattern();

char *ExpiresHeaderGetName(struct ExpiresHeader *e);
int ExpiresHeaderGetExpires(struct ExpiresHeader *e);
