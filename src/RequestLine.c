#include "RequestLine.h"

#include <string.h>
#include <stdio.h>

#define SPACE 0x20

int parseRequestLine(char *requestLineString, struct RequestLine *requestLine)
{
    char *method = requestLineString;
    char *uri ;

    char *curr = requestLineString;
    
    while( *curr != SPACE) {
        curr ++;
    }
    strncpy(requestLine->Method, method,(int) (curr - method)); 
   
    uri = ++curr;
    while (*curr != SPACE) {
        curr ++;
    }
    strncpy(requestLine->Request_URI, uri, (int) (curr - uri));

    sprintf(requestLine->SIP_Version, "SIP/2.0");
}
