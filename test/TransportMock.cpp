#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include <stdio.h>
#include <string.h>
}
int ReceiveMessageMock(char *message)
{
    strcpy(message, mock().actualCall("ReceiveMessageMock").returnStringValue());
    return 0;
}

int SendMessageMock(char *message)
{
    return mock().actualCall("SendMessageMock").returnIntValue();
}
