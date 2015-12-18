#include "Bool.h"

struct ContactHeader;
struct Parameters;

struct HeaderPattern *GetContactHeaderPattern(char *header);
struct ContactHeader *CreateContactHeader();
struct ContactHeader *CreateToHeader();
struct ContactHeader *CreateFromHeader();
void DestoryContactHeader(struct Header *to);
struct Header *ParseContactHeader(char *string);

char *ContactHeaderGetName(struct ContactHeader *Header);
char *ContactHeaderGetDisplayName(struct ContactHeader *Header);
struct URI *ContactHeaderGetUri(struct ContactHeader *Header);
char *ContactHeaderGetParameter(struct ContactHeader *Header, char *name);
struct Parameters *ContactHeaderGetParameters(struct ContactHeader *header);
void ContactHeaderSetDisplayName(struct ContactHeader *header, char *displayName);
void ContactHeaderSetUri(struct ContactHeader *header, struct URI *uri);
void ContactHeaderSetParameters(struct ContactHeader *header, struct Parameters *parameters);
BOOL ContactHeaderMatched(struct ContactHeader *header1, struct ContactHeader *header2);

char *ContactHeader2String(char *result, struct Header *contact);






