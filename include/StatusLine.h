#include "SipMethod.h"
struct StatusLine;

char *GetSIPVersion(struct StatusLine *s);
int GetStatusCode(struct StatusLine *s);
char *GetReasonPhrase(struct StatusLine *s);
char *StatusLine2String(char *result, struct StatusLine *s);

struct StatusLine *CreateStatusLine(int statusCode, const char *reasonPhrase);
void DestroyStatusLine(struct StatusLine *s);

void ParseStatusLine(char *string, struct StatusLine *s);
