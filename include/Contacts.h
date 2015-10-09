struct ContactHeader;

struct HeaderPattern *GetContactsHeaderPattern(char *header);
struct ContactHeader *CreateContactsHeader();
void DestoryContactsHeader(struct Header *to);
struct Header *ParseContactsHeader(char *string);

char *ContactsHeaderGetName(struct ContactHeader *Header);
char *ContactsHeaderGetDisplayName(struct ContactHeader *Header);
char *ContactsHeaderGetUri(struct ContactHeader *Header);
char *ContactsHeaderGetParameters(struct ContactHeader *Header);

void ContactsHeaderSetName(struct ContactHeader *header, char *name);
void ContactsHeaderSetDisplayName(struct ContactHeader *header, char *displayName);

char *ContactsHeader2String(char *result, struct Header *contact);
