#include "Bool.h"
#include "SipMethod.h"

#define MAX_MESSAGE_LENGTH 2048

typedef struct Message MESSAGE;
enum MESSAGE_TYPE {
    MESSAGE_TYPE_REQUEST,
    MESSAGE_TYPE_RESPONSE,
    MESSAGE_TYPE_NONE
};

struct Message;

int ParseMessage(const char *string, struct Message *message);
struct Message *CreateMessage();
void DestroyMessage(struct Message **message);

enum MESSAGE_TYPE MessageGetType(struct Message *message);
SIP_METHOD MessageGetMethod(struct Message *message);
void MessageSetType(struct Message *message, enum MESSAGE_TYPE type);
struct RequestLine *MessageGetRequestLine(struct Message *message);
struct StatusLine *MessageGetStatusLine(struct Message *message);
void MessageSetRequestLine(struct Message *message, struct RequestLine *rl);
void MessageSetStatusLine(struct Message *message, struct StatusLine *status);
struct Header *MessageGetHeader(const char *name, struct Message *message);
void MessageAddHeader(struct Message *message, struct Header *header);

char *MessageGetCallId(struct Message *message);
char *MessageGetFromTag(struct Message *message);
char *MessageGetToTag(struct Message *message);
void MessageSetRemoteTag(struct Message *message, char *tag);
int MessageGetExpires(struct Message *message);
unsigned int MessageGetCSeqNumber(struct Message *message);
char *MessageGetViaBranch(struct Message *message);
void MessageSetViaBranch(struct Message *message, char *branch);
char *MessageGetCSeqMethod(struct Message *message);

struct Transaction *MessageBelongTo(struct Message *message);
void MessageSetOwner(struct Message *message, struct Transaction *t);

void MessageAddViaParameter(struct Message *message, char *name, char *value);
void MessageSetCSeqMethod (struct Message *message, char *method);
void MessageSetContentLength(struct Message *message, int length);
unsigned int MessageGetContentLength(struct Message *message);


void ExtractHeaderName(char *header, char *name);
void Message2String(char *result, struct Message *message);
void MessageDump(struct Message *message);

BOOL MessageViaHeaderBranchMatched(struct Message *m, struct Message *mm);
BOOL MessageViaHeaderSendbyMatched(struct Message *m, struct Message *mm);
BOOL MessageCSeqHeaderMethodMatched(struct Message *m, struct Message *mm);

