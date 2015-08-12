
struct URI {
    char scheme[8];
    char user[16];
    char password[16];
};

int ParseURI(char *URIString, struct URI *uri);
