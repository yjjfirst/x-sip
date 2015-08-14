
#define SPACE " "
#define CRLF "\r\n"
#define SEMICOLON ";"
#define COLON ":"
#define AT "@"
#define QUESTION "?"
#define EMPTY ""

struct ParsePattern {
    char *name;
    char *token;
    int maybe_last;
    unsigned long offset;
};

#define OFFSETOF(type, field)  ((unsigned long) &(((type *) 0)->field))

int Parse(char *header, void *target, struct ParsePattern *pattern);


