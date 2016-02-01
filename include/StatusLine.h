#include "SipMethod.h"
struct StatusLine;

#define STATUS_CODE_TRYING 100
#define STATUS_CODE_RINGING 180
#define STATUS_CODE_OK 200

#define REASON_PHRASE_TRYING "Trying"
#define REASON_PHRASE_RINGING "Ringing"
#define REASON_PHRASE_OK "OK"

char *StatusLineGetSipVersion(struct StatusLine *s);
int StatusLineGetStatusCode(struct StatusLine *s);
char *StatusLineGetReasonPhrase(struct StatusLine *s);
char *StatusLine2String(char *result, struct StatusLine *s);

struct StatusLine *CreateStatusLine(int statusCode, const char *reasonPhrase);
void DestoryStatusLine(struct StatusLine *s);

void ParseStatusLine(char *string, struct StatusLine *s);
