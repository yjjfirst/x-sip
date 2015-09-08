struct ContactHeader;

struct ParsePattern *GetContactsHeaderPattern(char *header);
struct ContactHeader *CreateContactsHeader();
void DestoryContactsHeader(struct ContactHeader *to);

char *ContactsHeaderGetName(struct ContactHeader *toHeader);
char *ContactsHeaderGetDisplayName(struct ContactHeader *toHeader);
char *ContactsHeaderGetUri(struct ContactHeader *toHeader);
char *ContactsHeaderGetParameters(struct ContactHeader *toHeader);

