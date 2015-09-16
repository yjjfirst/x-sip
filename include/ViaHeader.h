struct ViaHeader;
struct Header;

struct HeaderPattern *GetViaPattern();
struct ViaHeader *CreateViaHeader();
void DestoryViaHeader(struct Header *via);
struct Header *ParseViaHeader(char *string);

char *ViaHeaderGetUri(struct ViaHeader *via);
char *ViaHeaderGetTransport(struct ViaHeader *via);
char *ViaHeaderGetName(struct ViaHeader *via);
char *ViaHeaderGetParameters(struct ViaHeader *via);

void ViaHeader2String(char *result, struct ViaHeader *via);
