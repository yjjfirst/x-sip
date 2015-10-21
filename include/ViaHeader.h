struct ViaHeader;
struct Header;
struct URI;

struct HeaderPattern *GetViaPattern();
struct ViaHeader *CreateEmptyViaHeader();
struct ViaHeader *CreateViaHeader(struct URI *uri);
void DestoryViaHeader(struct Header *via);
struct Header *ParseViaHeader(char *string);

struct URI *ViaHeaderGetUri(struct ViaHeader *via);
char *ViaHeaderGetTransport(struct ViaHeader *via);
char *ViaHeaderGetName(struct ViaHeader *via);
char *ViaHeaderGetParameters(struct ViaHeader *via);

void ViaHeaderSetUri(struct ViaHeader *via, struct URI *uri);

char *ViaHeader2String(char *result, struct Header *via);
