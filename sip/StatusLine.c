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
    {"*", EMPTY, SPACE, 0, OFFSETOF(struct StatusLine, sipVersion), ParseString, NULL, String2String},
    {"*", SPACE, SPACE, 0, OFFSETOF(struct StatusLine, statusCode), ParseInteger, NULL, Integer2String},
    {"*", SPACE, EMPTY, 0, OFFSETOF(struct StatusLine, reasonPhrase), ParseString, NULL,String2String},
    {NULL, 0, 0, 0, 0, 0}

};

void ParseStatusLine(char *string, struct StatusLine *s)
{
    Parse(string, s, StatusLinePattern); 
}

char *StatusLineGetSipVersion(struct StatusLine *s)
{
    return s->sipVersion;
}

void StatusLineSetSipVersion(struct StatusLine *s, char *version)
{
    Copy2Target(s, version, &StatusLinePattern[0]);
}

int StatusLineGetStatusCode(struct StatusLine *s)
{
    return s->statusCode;
}

void StatusLineSetStatusCode(struct StatusLine *s, int statusCode)
{
    SetIntegerField(s, statusCode, &StatusLinePattern[1]);
}

char *StatusLineGetReasonPhrase(struct StatusLine *s)
{
    return s->reasonPhrase;
}

void StatusLineSetReasonPhrase(struct StatusLine *s, char *reasonPhrase)
{
    
    Copy2Target(s, reasonPhrase, &StatusLinePattern[2]);
}

char *StatusLine2String(char *result, struct StatusLine *s)
{
    return ToString(result, s, StatusLinePattern);
}

struct StatusLine *CreateStatusLine(int statusCode, const char *reasonPhrase)
{
    struct StatusLine *s = calloc(1, sizeof(struct StatusLine));
    StatusLineSetSipVersion(s, SIP_VERSION);
    StatusLineSetStatusCode(s, statusCode);
    if (reasonPhrase != NULL)
        StatusLineSetReasonPhrase(s, (char *)reasonPhrase);
    return s;
}

DEFINE_DESTROYER(struct StatusLine, DestoryStatusLine)
