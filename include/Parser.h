
#define SPACE " "
#define CRLF "\r\n"
#define SEMICOLON ";"
#define COLON ":"
#define AT "@"

struct ParsePattern {
    char *name;
    char *separator;
    int optional;
    unsigned int offset;
};

#define OFFSETOF(type, field)  ((unsigned long) &(((type *) 0)->field))

int Parse(char *header, void *target, struct ParsePattern *pattern);


