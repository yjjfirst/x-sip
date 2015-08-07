#include "RequestLine.h"

int parseRequestLine(char *requestLineString, struct RequestLine *requestLine)
{
    sprintf(requestLine->Method, "REGISTER");
    sprintf(requestLine->Request_URI, "sip:192.168.2.89");
    sprintf(requestLine->SIP_Version, "SIP/2.0");
}
