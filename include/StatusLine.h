#include "SipMethod.h"
struct StatusLine;

char *StatusLineGetSipVersion(struct StatusLine *s);
int StatusLineGetStatusCode(struct StatusLine *s);
char *StatusLineGetReasonPhrase(struct StatusLine *s);
char *StatusLine2String(char *result, struct StatusLine *s);

struct StatusLine *CreateStatusLine(int statusCode, const char *reasonPhrase);
void DestoryStatusLine(struct StatusLine *s);

void ParseStatusLine(char *string, struct StatusLine *s);
