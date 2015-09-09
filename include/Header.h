#define HEADER_NAME_VIA "Via"
#define HEADER_NAME_MAX_FORWARDS "Max-Forwards"
#define HEADER_NAME_TO "To"
#define HEADER_NAME_FROM "From"
#define HEADER_NAME_CONTACT "Contact"

#define HEADER_NAME_MAX_LENGTH 32
struct Header {
    char name[HEADER_NAME_MAX_LENGTH];
};

static inline char *HeaderGetName(struct Header *header)
{
    return header->name;
}
