#include "Parser.h"
struct ToHeader {
    char name[16];
    char displayName[32];
    char uri[128];
    char parameters[128];
};

struct ParsePattern *GetToHeaderPattern();
