#define SPACE 0x20
#define CRLF        "\r\n"
#define SEMICOLON   0x3b
#define COLON       0x3a
#define AT          0x40
#define QUESTION    0x3f
#define LEFT_ANGLE  0x3c
#define RIGHT_ANGLE 0x3e
#define QUOTE       0x22
#define EMPTY       0x00
#define ANY         0x7F

struct HeaderPattern {
    char *format;
    char startSeparator;
    char endSeparator;
    int  mandatory;
    unsigned long offset;
    int (*parse)(char *header, void *target);
    int (*legal)(char *value);
    char *(*toString)(char *string, void *element, struct HeaderPattern *p);
};

#define OFFSETOF(type, field)  ((unsigned long) &(((type *) 0)->field))

#define DEFINE_CREATER(type, name)              \
    type *name ()                               \
    {                                           \
        return (type *)calloc(1,sizeof (type)); \
    }  

#define DEFINE_DESTROYER(type, name)            \
    void name (type *header)                    \
    {                                           \
        if (header != NULL)                     \
            free(header);                       \
    }           

int Parse(char *string, void *target, struct HeaderPattern *pattern);
char *ToString(char *string, void *header, struct HeaderPattern *pattern);
char *NextSeparator(char *header);
int ParseStringElement(char *value, void *target);
int ParseIntegerElement(char *value, void *target);
char *StringElement2String(char *pos, void *element, struct HeaderPattern *p);
