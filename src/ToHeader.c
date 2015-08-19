#include <stdio.h>
#include "ToHeader.h"


struct ParsePattern ToHeaderPattern[] = {
    { "name", EMPTY, COLON, 0, OFFSETOF(struct ToHeader, name)}, 
    { "displayName",  COLON, LEFT_ANGLE, 0, OFFSETOF(struct ToHeader, displayName)}, 
    { "uri", LEFT_ANGLE, RIGHT_ANGLE, 0, OFFSETOF(struct ToHeader, uri)},
    { "empty", RIGHT_ANGLE, SEMICOLON, 1, 0},
    { "parameters", SEMICOLON, EMPTY, 0, OFFSETOF(struct ToHeader, parameters)},
    {NULL, 0, 0, 0}
};

struct ParsePattern *GetToHeaderPattern()
{
    return ToHeaderPattern;
}
