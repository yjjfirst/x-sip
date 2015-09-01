#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Messages.h"

struct Message {
    struct RequestLine *request;
};

void ParseRequestLine(char *string, struct Message *message)
{
    struct RequestLine *requestLine = CreateRequestLine();

    Parse(string, requestLine, GetRequestLinePattern()); 
    message->request = requestLine;
}

void ParseHeader(char *headerString)
{
}

int ParseMessage(char *string, struct Message *message)
{
    char *rawMessage = strdup (string);
    char *line = strtok(rawMessage, "\r\n");
    
    ParseRequestLine(line, message);
    line = strtok(NULL, "\r\n");  
    while(line) {
        ParseHeader(line);
        line = strtok(NULL, "\r\n");
    }
    
    free (rawMessage);
    return 0;
}

struct RequestLine *MessageGetRequest(struct Message *message)
{
    return message->request;
}

struct Message *CreateMessage () 
{ 
    return (struct Message *)calloc(1,sizeof (struct Message)); 
}

void DestoryMessage (struct Message *message) 
{ 
    if (message != ((void *)0)) {
        DestoryRequestLine(message->request);
        free(message); 
    }
}

