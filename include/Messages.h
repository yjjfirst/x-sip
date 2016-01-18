#include "Bool.h"

#define MAX_MESSAGE_LENGTH 2048

enum MESSAGE_TYPE {
    MESSAGE_TYPE_REQUEST,
    MESSAGE_TYPE_RESPONSE,
    MESSAGE_TYPE_NONE
};

struct Message;

int ParseMessage(char *string, struct Message *message);
struct Message *CreateMessage();
void DestoryMessage(struct Message **message);

enum MESSAGE_TYPE MessageGetType(struct Message *message);
void MessageSetType(struct Message *message, enum MESSAGE_TYPE type);
struct RequestLine *MessageGetRequestLine(struct Message *message);
struct StatusLine *MessageGetStatusLine(struct Message *message);
void MessageSetRequestLine(struct Message *message, struct RequestLine *rl);
void MessageSetStatusLine(struct Message *message, struct StatusLine *status);

char *MessageGetCallId(struct Message *message);
char *MessageGetLocalTag(struct Message *message);
char *MessageGetRemoteTag(struct Message *message);
void MessageSetRemoteTag(struct Message *message, char *tag);
int MessageGetExpires(struct Message *message);
struct Header *MessageGetHeader(const char *name, struct Message *message);
unsigned int MessageGetCSeqNumber(struct Message *message);
char *MessageGetViaBranch(struct Message *message);
char *MessageGetCSeqMethod(struct Message *message);

void MessageAddHeader(struct Message *message, struct Header *header);
void MessageAddViaParameter(struct Message *message, char *name, char *value);
void MessageSetCSeqMethod (struct Message *message, char *method);
void MessageSetContentLength(struct Message *message, int length);

void  ExtractHeaderName(char *header, char *name);
void Message2String(char *result, struct Message *message);
void MessageDump(struct Message *message);

BOOL RequestResponseMatched(struct Message *request, struct Message *response);
