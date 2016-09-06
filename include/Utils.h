#include <assert.h>
#include <string.h>

#define DEFINE_STRING_MEMBER_WRITER(struct, name, field, maxLength)     \
    void name (struct *object, char *field)                             \
    {                                                                   \
        assert(object != NULL);                                         \
        bzero(object->field, sizeof(object->field));                    \
        if (field != NULL)                                              \
            strncpy(object->field, field, sizeof(object->field) - 1);   \
    }                                                                   \
    
#define DEFINE_STRING_MEMBER_READER(struct, name, field)     \
    char *name (struct *object)                              \
    {                                                        \
        assert(object != NULL);                              \
        return object->field;                                \
    }                                                        \

