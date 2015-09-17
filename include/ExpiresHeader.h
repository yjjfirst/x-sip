#include "Parser.h"

struct ExpiresHeader;

struct ExpiresHeader *CreateExpiresHeader();
void DestoryExpiresHeader(struct ExpiresHeader *e);
struct HeaderPattern *GetExpiresPattern();

char *ExpiresHeaderGetName(struct ExpiresHeader *e);
int ExpiresHeaderGetExpires(struct ExpiresHeader *e);

void ExpiresHeader2String(char *result, struct ExpiresHeader *e);
