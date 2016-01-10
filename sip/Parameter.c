#include <string.h>
#include <stdio.h>

#include "Header.h"
#include "Parser.h"
#include "Parameter.h"
#include "Utils.h"
#include "utils/list/include/list.h"


#define PARAMETER_MAX_NAME_LENGTH 32
#define PARAMETER_MAX_VALUE_LENGTH 128

struct Parameters {
    t_list *parameters;
};

struct Parameter {
    char name[PARAMETER_MAX_NAME_LENGTH];
    char value[PARAMETER_MAX_VALUE_LENGTH];
};

static struct HeaderPattern ParameterPattern[] = {
    {"*", EMPTY, EQUAL, 1, OFFSETOF(struct Parameter, name), ParseString, NULL, String2String},
    {"*", EQUAL, EMPTY, 0, OFFSETOF(struct Parameter,value), ParseString, NULL, String2String},
    {NULL},
};

struct Parameter *ParseParameter(char *s)
{
    struct Parameter *p = CreateEmptyParameter();
    Parse(s, p, ParameterPattern);
    
    return p;
}

BOOL ParameterMatched(struct Parameter *p1, struct Parameter *p2)
{
    return !(strcmp(p1->name, p2->name) != 0 
            || strcmp(p1->value, p2->value) != 0);
}

DEFINE_STRING_MEMBER_READER(struct Parameter, GetParameterName, name);
DEFINE_STRING_MEMBER_READER(struct Parameter, GetParameterValue, value);
/*
 * Only call this function when parse parameters stand alone.
 */
int ParseParametersExt(char *s, void *target)
{
    struct Parameters *p = (struct Parameters *)target;    
    char *localString = calloc(1, strlen(s) + 1);    
    char *paramStart;
    char *save_ptr = NULL;
   
    strcpy(localString, s);

    paramStart = strtok_r(localString, ";", &save_ptr);

    while(paramStart) {
        put_in_list(&p->parameters, ParseParameter(paramStart));
        paramStart = strtok_r(NULL, ";", &save_ptr);
    }
    
    free(localString);

    return 0;

}

/* 
 * Only call this function when parameters embedded in other structure 
 */
int ParseParameters(char *s, void *target)
{
    struct Parameters **p = target;

    ParseParametersExt(s, *p);

    return 0;
}

int AddParameter(struct Parameters *ps, char *name, char *value)
{
    struct Parameter *p = NULL;
    int i = 0;
    int length = get_list_len(ps->parameters);

    for ( ; i < length; i ++) {
        if (strcmp (((struct Parameter *) get_data_at(ps->parameters, i))->name, name) == 0)
            p = get_data_at (ps->parameters, i);
    }

    if (p == NULL) {
        p = CreateEmptyParameter();
        put_in_list(&ps->parameters, p);
    }

    Copy2Target(p, name, &ParameterPattern[0]);
    Copy2Target(p, value,&ParameterPattern[1]);

    return 0;
}

BOOL ParametersMatched(struct Parameters *ps1, struct Parameters *ps2)
{
    int i = 0;

    assert(ps1 != NULL);
    assert(ps2 != NULL);

    int length = get_list_len(ps1->parameters);    
    
    if (length != get_list_len(ps2->parameters)) {
        return FALSE;
    }

    for ( ; i < length; i ++) {
        struct Parameter *p1 = get_data_at(ps1->parameters, i);
        struct Parameter *p2 = get_data_at(ps2->parameters, i);
        if (!ParameterMatched(p1, p2))
            return FALSE;

    }

    return TRUE;
    
}

char *GetParameter(struct Parameters *ps, char *name)
{
    int i = 0;
    int length = get_list_len(ps->parameters);

    for ( ; i < length; i ++) {
        struct Parameter *p = get_data_at(ps->parameters, i);
        if (strcmp (p->name, name) == 0)
            return p->value;
    }

    return NULL;
}

char *Parameters2String(char *pos, void *ps, struct HeaderPattern *p)
{
    struct Parameters **params = ps;
    
    return Parameters2StringExt(pos, *params, p);
}

char *Parameters2StringExt(char *pos, void *ps, struct HeaderPattern *pattern)
{
    int i = 0;
    struct Parameters *params = (struct Parameters *)ps;
    int length = get_list_len(params->parameters);

    for (; i < length; i ++) {
        struct Parameter *p = get_data_at(params->parameters, i);

        if (i == 0 && pattern != NULL)
            *pos++ = pattern->startSeparator;
        else
            *pos ++ = SEMICOLON;
        pos = ToString(pos, p, ParameterPattern);
    }

    return pos;
}

struct Parameter *CreateEmptyParameter()
{
    struct Parameter *p = calloc(1, sizeof(struct Parameter));
    
    assert(p != NULL);
    return p;
}

struct Parameter *CreateParameter(char *name, char *value)
{
    struct Parameter *p = CreateEmptyParameter();

    assert(name != NULL);
    assert(value != NULL);

    Copy2Target(p, name, &ParameterPattern[0]);
    Copy2Target(p, value,&ParameterPattern[1]);

    return p;
}

void DestoryParameter(struct Parameter *p)
{
    if (p != NULL)
        free(p);
}

struct Parameters *CreateParameters()
{
    return calloc(1, sizeof(struct Parameters));
}

struct Parameters *ParametersDup(struct Parameters *src)
{
    struct Parameters *dest = CreateParameters();
    int length = get_list_len(src->parameters);
    int i = 0;
    
    for (; i < length; i++) {
        struct Parameter *p = get_data_at(src->parameters, i);
        AddParameter(dest, GetParameterName(p), GetParameterValue(p));
    }
    return dest;
}

void DestoryParameters(struct Parameters *ps)
{
    if (ps == NULL) return;

    int length = get_list_len(ps->parameters);
    int i = 0;
    for ( ; i < length; i ++) {
        struct Parameter *p = get_data_at(ps->parameters, i);
        free(p);
    }

    free(ps);
}
