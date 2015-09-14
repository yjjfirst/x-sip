struct ContactHeader;

struct HeaderPattern *GetContactsHeaderPattern(char *header);
struct ContactHeader *CreateContactsHeader();
void DestoryContactsHeader(struct Header *to);
struct Header *ParseContactsHeader(char *string);

char *ContactsHeaderGetName(struct ContactHeader *toHeader);
char *ContactsHeaderGetDisplayName(struct ContactHeader *toHeader);
char *ContactsHeaderGetUri(struct ContactHeader *toHeader);
char *ContactsHeaderGetParameters(struct ContactHeader *toHeader);

