#include "Bool.h"
#include "SipMethod.h"

#define MAX_MESSAGE_LENGTH 2048

#define STATUS_CODE_TRYING 100
#define STATUS_CODE_RINGING 180
#define STATUS_CODE_OK 200
#define STATUS_CODE_MOVED_PERMANENTLY 301
#define STATUS_CODE_REQUEST_TERMINATED 487

#define REASON_PHRASE_TRYING "Trying"
#define REASON_PHRASE_RINGING "Ringing"
#define REASON_PHRASE_OK "OK"
#define REASON_PHRASE_MOVED_PERMANENTLY "Moved Permanently"
#define REASON_PHRASE_REQUEST_TERMINATED "Request Terminated"

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
void SetMessageType(struct Message *message, enum MESSAGE_TYPE type);
struct RequestLine *MessageGetRequestLine(struct Message *message);
struct StatusLine *MessageGetStatusLine(struct Message *message);
void SetMessageRequestLine(struct Message *message, struct RequestLine *rl);
void SetMessageStatusLine(struct Message *message, struct StatusLine *status);
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


char *GetMessageAddr(MESSAGE *message);
void SetMessageAddr(MESSAGE *m, char *addr);
int  GetMessagePort(MESSAGE *message);
void SetMessagePort(MESSAGE *m, int port);
