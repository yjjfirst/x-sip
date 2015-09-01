#include "RequestLine.h"

struct Message;

int ParseMessage(char *string, struct Message *message);
struct Message *CreateMessage();
void DestoryMessage(struct Message **message);

struct RequestLine *MessageGetRequest(struct Message *message);
