#include "Parser.h"

struct ToHeader;

struct ParsePattern *GetContactsHeaderPattern(char *header);
struct ToHeader *CreateContactsHeader();
void DestoryContactsHeader(struct ToHeader *to);

char *ContactsHeaderGetName(struct ToHeader *toHeader);
char *ContactsHeaderGetDisplayName(struct ToHeader *toHeader);
char *ContactsHeaderGetUri(struct ToHeader *toHeader);
char *ContactsHeaderGetParameters(struct ToHeader *toHeader);

