#include "URI.h"
struct ContactHeader;

struct HeaderPattern *GetContactsHeaderPattern(char *header);
struct ContactHeader *CreateContactsHeader();
void DestoryContactsHeader(struct Header *to);
struct Header *ParseContactsHeader(char *string);

char *ContactsHeaderGetName(struct ContactHeader *Header);
char *ContactsHeaderGetDisplayName(struct ContactHeader *Header);
struct URI *ContactsHeaderGetUri(struct ContactHeader *Header);
char *ContactsHeaderGetParameters(struct ContactHeader *Header);

void ContactsHeaderSetName(struct ContactHeader *header, char *name);
void ContactsHeaderSetDisplayName(struct ContactHeader *header, char *displayName);
void ContactsHeaderSetUri(struct ContactHeader *header, struct URI *uri);

char *ContactsHeader2String(char *result, struct Header *contact);
