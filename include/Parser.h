
#define SPACE " "
#define CRLF "\r\n"

struct ParsePattern {
    char *name;
    char *separator;
    int optional;
    int (*parser) (char *value, void *target);
};

int parse(char *header, void *target, struct ParsePattern *pattern);


