#include "RequestLine.h"

struct Message;
#define MAX_MESSAGE_LENGTH 2048

int ParseMessage(char *string, struct Message *message);
struct Message *CreateMessage();
void DestoryMessage(struct Message **message);

struct RequestLine *MessageGetRequestLine(struct Message *message);
struct StatusLine *MessageGetStatusLine(struct Message *message);
char *MessageGetCallId(struct Message *message);
char *MessageGetLocalTag(struct Message *message);
char *MessageGetRemoteTag(struct Message *message);
void MessageSetRemoteTag(struct Message *message, char *tag);
int MessageGetExpires(struct Message *message);
struct Header *MessageGetHeader(const char *name, struct Message *message);
unsigned int MessageGetCSeqNumber(struct Message *message);

void MessageSetRequest(struct Message *message, struct RequestLine *rl);
void MessageAddHeader(struct Message *message, struct Header *header);
void MessageAddViaParameter(struct Message *message, char *name, char *value);
void MessageSetCSeqMethod (struct Message *message, char *method);
void MessageSetContentLength(struct Message *message, int length);

void  ExtractHeaderName(char *header, char *name);

void Message2String(char *result, struct Message *message);
void MessageDump(struct Message *message);
