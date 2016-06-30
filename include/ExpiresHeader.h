struct ExpiresHeader;
struct Header;

struct ExpiresHeader *CreateExpiresHeader(int expires);
void DestroyExpiresHeader(struct Header *e);
struct HeaderPattern *GetExpiresPattern();

char *ExpiresHeaderGetName(struct ExpiresHeader *e);
int ExpiresHeaderGetExpires(struct ExpiresHeader *e);

char *ExpiresHeader2String(char *result, struct Header *e);
struct Header *ParseExpiresHeader(char *string);
