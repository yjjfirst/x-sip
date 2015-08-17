
#define SPACE 0x20
#define CRLF "\r\n"
#define SEMICOLON 0x3b
#define COLON     0x3a
#define AT        0x40
#define QUESTION  0x3f
#define EMPTY     0x00
#define ANY       0x7F

struct ParsePattern {
    char *name;
    char startToken;
    char endToken;
    int  optional;
    unsigned long offset;
};

#define OFFSETOF(type, field)  ((unsigned long) &(((type *) 0)->field))

int Parse(char *header, void *target, struct ParsePattern *pattern);


