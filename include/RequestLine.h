#include "URI.h"

typedef enum {
    SIP_METHOD_INVITE,
    SIP_METHOD_ACK,
    SIP_METHOD_OPTIONS,
    SIP_METHOD_BYE,
    SIP_METHOD_CANCEL,
    SIP_METHOD_REGISTER,
    SIP_METHOD_INFO,
    SIP_METHOD_PRACK,
    SIP_METHOD_SUBSCRIBE,
    SIP_METHOD_NOTIFY,
    SIP_METHOD_UPDATE,
    SIP_METHOD_MESSAGE,
    SIP_METHOD_REFER,
    SIP_METHOD_PUBLISH,
} SIP_METHOD;

struct RequestLine;

void DestoryRequestLine(struct RequestLine *requestLine);
struct RequestLine *CreateEmptyRequestLine();
struct RequestLine *CreateRequestLine(SIP_METHOD m, struct URI *u);

char *RequestLineGetMethod (struct RequestLine *r);
char *RequestLineGetSipVersion(struct RequestLine *r);
struct URI *RequestLineGetUri(struct RequestLine *r);

int RequestLineSetMethod(struct RequestLine *r, char *method);
int RequestLineSetSipVersion(struct RequestLine *r, char *version);
int RequestLineSetUri(struct RequestLine *r, struct URI *u);

char *RequestLine2String(char *string, struct RequestLine *r);
int ParseRequestLine(char *string, struct RequestLine *r);









