#include "Bool.h"


#define VIA_BRANCH_PARAMETER_NAME "branch"
struct ViaHeader;
struct Header;
struct URI;
struct Parameters;

struct HeaderPattern *GetViaPattern();
struct ViaHeader *CreateEmptyViaHeader();
struct ViaHeader *CreateViaHeader(struct URI *uri);
struct ViaHeader *ViaHeaderDup(struct ViaHeader *src);
void DestoryViaHeader(struct Header *via);
struct Header *ParseViaHeader(char *string);

struct URI *ViaHeaderGetUri(struct ViaHeader *via);
char *ViaHeaderGetTransport(struct ViaHeader *via);
char *ViaHeaderGetName(struct ViaHeader *via);
char *ViaHeaderGetParameter(struct ViaHeader *via, char *name);
struct Parameters *ViaHeaderGetParameters(struct ViaHeader *via);

void ViaHeaderSetUri(struct ViaHeader *via, struct URI *uri);
void ViaHeaderSetParameters(struct ViaHeader *via, struct Parameters *parameters);

char *ViaHeader2String(char *result, struct Header *via);
BOOL ViaHeaderBranchMatched(struct ViaHeader *via1, struct ViaHeader *via2);
BOOL ViaHeaderBranchMatchedByString(struct ViaHeader *via, char *string);
BOOL ViaHeaderMatched(struct ViaHeader *via1, struct ViaHeader *via2);
