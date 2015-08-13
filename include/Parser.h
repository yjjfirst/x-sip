
#define SPACE " "
#define CRLF "\r\n"
#define SEMICOLON ";"
#define COLON ":"
#define AT "@"

struct ParsePattern {
    char *name;
    char *separator;
    int optional;
    int (*parser) (char *value, void *target);
};

int Parse(char *header, void *target, struct ParsePattern *pattern);


