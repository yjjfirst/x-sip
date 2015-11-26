#include <assert.h>
#include <string.h>
#include "UserAgent.h"

struct UserAgent {
    char userName[USER_NAME_MAX_LENGTH];
    char authName[AUTH_NAME_MAX_LENGTH];
    char proxy[PROXY_MAX_LENGTH];
    char registrar[REGISTRAR_MAX_LENGTH];
};

#define DEFINE_STRING_MEMBER_WRITER(struct, name, field, maxLength)    \
    void name (struct *ua, char *field)                                \
    {                                                                  \
        assert(ua != NULL);                                            \
        strncpy(ua->field, field, sizeof(ua->field) - 1);              \
        ua->field[maxLength - 1] = '\0';                               \
    }                                                                  \
    
#define DEFINE_STRING_MEMBER_READER(struct, name, field) \
    char *name (struct *ua)                              \
    {                                                    \
        assert(ua != NULL);                              \
        return ua->field;                                \
    }                                                    \

DEFINE_STRING_MEMBER_WRITER(struct UserAgent, UserAgentSetUserName, userName, USER_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct UserAgent, UserAgentGetUserName,userName);

DEFINE_STRING_MEMBER_WRITER(struct UserAgent, UserAgentSetProxy, proxy, PROXY_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct UserAgent, UserAgentGetProxy, proxy);

DEFINE_STRING_MEMBER_WRITER(struct UserAgent, UserAgentSetRegistrar, registrar, REGISTRAR_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct UserAgent, UserAgentGetRegistrar, registrar);

DEFINE_STRING_MEMBER_WRITER(struct UserAgent, UserAgentSetAuthName, authName, AUTH_NAME_MAX_LENGTH);
DEFINE_STRING_MEMBER_READER(struct UserAgent, UserAgentGetAuthName, authName);

void UserAgentAddBindings(struct UserAgent *ua)
{

}

struct UserAgent *CreateUserAgent()
{
    struct UserAgent *ua = calloc(1, sizeof(struct UserAgent));
    return ua;
}

void DestoryUserAgent(struct UserAgent **ua)
{
    if (*ua != NULL) {
        free(*ua);
        *ua = NULL;
    }
}
