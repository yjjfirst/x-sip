#include <stdio.h>
#include <stdlib.h>
#include "Parser.h"
#include "StatusLine.h"

struct StatusLine {
    char sipVersion[16];
    int statusCode;
    char reasonPhrase[64];
};

struct HeaderPattern StatusLinePattern[] = {
    {"*", EMPTY, SPACE, 0, OFFSETOF(struct StatusLine, sipVersion), ParseStringElement, NULL, StringElement2String},
    {"*", SPACE, SPACE, 0, OFFSETOF(struct StatusLine, statusCode), ParseIntegerElement, NULL, IntegerElement2String},
    {"*", SPACE, EMPTY, 0, OFFSETOF(struct StatusLine, reasonPhrase), ParseStringElement, NULL,StringElement2String},
    {NULL, 0, 0, 0, 0, 0}

};

DEFINE_CREATER(struct StatusLine, CreateStatusLine)
DEFINE_DESTROYER(struct StatusLine, DestoryStatusLine)

void ParseStatusLine(char *string, struct StatusLine *s)
{
    Parse(string, s, StatusLinePattern); 
}

char *StatusLineGetSipVersion(struct StatusLine *s)
{
    return s->sipVersion;
}

int StatusLineGetStatusCode(struct StatusLine *s)
{
    return s->statusCode;
}

char *StatusLineGetReasonPhrase(struct StatusLine *s)
{
    return s->reasonPhrase;
}

char *StatusLine2String(char *result, struct StatusLine *s)
{
    return ToString(result, s, StatusLinePattern);
}
