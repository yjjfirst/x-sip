#include "RequestLine.h"

struct Message;

int ParseMessage(char *string, struct Message *message);
struct Message *CreateMessage();
void DestoryMessage(struct Message **message);

struct RequestLine *MessageGetRequest(struct Message *message);
struct StatusLine *MessageGetStatus(struct Message *message);
struct Header *MessageGetHeader(const char *name, struct Message *message);

void MessageSetRequest(struct Message *message, struct RequestLine *rl);
void MessageAddHeader(struct Message *message, struct Header *header);

void  ExtractHeaderName(char *header, char *name);

void Message2String(char *result, struct Message *message);










