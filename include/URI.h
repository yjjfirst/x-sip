
struct URI {
    char scheme[8];
    char user[32];
    char host[32];
    char port[8];
    char parameters[128];
    char headers[128];
};

struct ParsePattern *GetURIParsePattern (char *header);
