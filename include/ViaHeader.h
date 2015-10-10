struct ViaHeader;
struct Header;

struct HeaderPattern *GetViaPattern();
struct ViaHeader *CreateEmptyViaHeader();
struct ViaHeader *CreateViaHeader(char *uri);
void DestoryViaHeader(struct Header *via);
struct Header *ParseViaHeader(char *string);

char *ViaHeaderGetUri(struct ViaHeader *via);
char *ViaHeaderGetTransport(struct ViaHeader *via);
char *ViaHeaderGetName(struct ViaHeader *via);
char *ViaHeaderGetParameters(struct ViaHeader *via);

void ViaHeaderSetUri(struct ViaHeader *via, char *uri);

char *ViaHeader2String(char *result, struct Header *via);
