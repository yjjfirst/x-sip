#include <stdio.h>
#include "Parser.h"

struct ViaHeader;

struct ParsePattern *GetViaPattern();
struct ViaHeader *CreateViaHeader();
void DestoryViaHeader(struct ViaHeader *via);
char *ViaHeaderGetUri(struct ViaHeader *via);
char *ViaHeaderGetTransport(struct ViaHeader *via);
char *ViaHeaderGetName(struct ViaHeader *via);
char *ViaHeaderGetParameters(struct ViaHeader *via);

