#include "CallEvents.h"

void (*NotifyClient)(enum CALL_EVENT event, struct UserAgent *ua) = NULL;
