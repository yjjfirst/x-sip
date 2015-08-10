#include "RequestLine.h"

#include <string.h>
#include <stdio.h>

#define SPACE 0x20

int parseRequestLine(char *requestLineString, struct RequestLine *requestLine)
{
    char *curr = requestLineString;
    char *start = requestLineString;

    curr = strchr(start, SPACE);
    strncpy(requestLine->Method, start,(int) (curr - start)); 
   
    start = ++curr;
    curr = strchr(start, SPACE);
    strncpy(requestLine->Request_URI, start, (int) (curr - start));

    start = ++curr;
    curr = strstr(start, "\r\n");
    strncpy(requestLine->SIP_Version, start, (int) (curr - start));

    return 0;
}
