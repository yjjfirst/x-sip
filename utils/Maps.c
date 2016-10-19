#include <stdio.h>
#include "Maps.h"
#include "StringExt.h"

char *IntMap2String(int integer, struct IntStringMap *map)
{
    return map[integer].string;
}

int StringMap2Int(char *string, struct IntStringMap *map)
{
    for ( ;map->method != -1; map++){
        if (StrcmpExt(map->string, string) == 0)
            return map->method;        
    }

    return -1;
}

