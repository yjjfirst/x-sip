#include <string.h>
#include <stdio.h>

#include "Header.h"
#include "Parser.h"
#include "Parameter.h"
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
    struct Parameter *p = CreateParameter();
    Parse(s, p, ParameterPattern);
    
    return p;
}

int ParseParameters(char *s, void *target)
{
    struct Parameters *p = (struct Parameters *)target;    
    char *localString = calloc(1, strlen(s) + 1);    
    char *paramStart;
    
    strcpy(localString, s);

    paramStart = strtok(localString, ";");

    while(paramStart) {
        put_in_list(&p->parameters, ParseParameter(paramStart));
        paramStart = strtok(NULL, ";");
    }
    
    free(localString);

    return 0;
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

char *Parameters2String(char *pos, void *ps, struct HeaderPattern *pattern)
{
    int i = 0;
    struct Parameters *params = (struct Parameters *)ps;
    int length = get_list_len(params->parameters);

    for (; i < length; i ++) {
        struct Parameter *p = get_data_at(params->parameters, i);
        pos = ToString(pos, p, ParameterPattern);
        if (i != length - 1 ) *pos ++ = ';';
    }

    return pos;
}

struct Parameter *CreateParameter()
{
    return calloc(1, sizeof(struct Parameter) );
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
