#include <stdio.h>
#include "Maps.h"
#include "StringExt.h"

char *IntMap2String(int integer, struct IntStringMap *map)
{    
    for ( ;map->integer != -1; map++){
        if (map->integer == integer)        
            return map->string;
    }
    
    return "UNKNOWN";
}

int StringMap2Int(char *string, struct IntStringMap *map)
{
    for ( ;map->integer != -1; map++){
        if (StrcmpExt(map->string, string) == 0)
            return map->integer;        
    }

    return -1;
}

