struct ContactHeader;

struct HeaderPattern *GetContactHeaderPattern(char *header);
struct ContactHeader *CreateContactHeader();
struct ContactHeader *CreateToHeader();
struct ContactHeader *CreateFromHeader();
void DestoryContactHeader(struct Header *to);
struct Header *ParseContactHeader(char *string);

char *ContactHeaderGetName(struct ContactHeader *Header);
char *ContactHeaderGetDisplayName(struct ContactHeader *Header);
struct URI *ContactHeaderGetUri(struct ContactHeader *Header);
char *ContactHeaderGetParameters(struct ContactHeader *Header);

void ContactHeaderSetDisplayName(struct ContactHeader *header, char *displayName);
void ContactHeaderSetUri(struct ContactHeader *header, struct URI *uri);

char *ContactHeader2String(char *result, struct Header *contact);
