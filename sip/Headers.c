#include <stdlib.h>

struct Headers {
};


struct Headers *CreateHeaders()
{
    return calloc(sizeof(struct Headers), 1);
}

void DestoryHeaders(struct Headers **headers)
{
    if (*headers != NULL) {
        free(*headers);
        *headers = NULL;
    }
}

int HeadersLength(struct Headers *headers)
{
    return 0;
}
