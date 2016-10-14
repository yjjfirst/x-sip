#include <string.h>
#include <stdio.h>

int StrcmpExt(const char *s1, const char *s2)
{
    if (s1 == NULL && s2 == NULL) return 0;
    if (s1 == NULL && s2 != NULL) return -1;
    if (s1 != NULL && s2 == NULL) return 1;

    return strcmp(s1, s2);
}

void RemoveTailingCRLF(char *s)
{
    char *p = s;
    
    while(*p != 0) {
        if (*p == '\r' || *p == '\n') *p = 0;
        p++;
    }

}
