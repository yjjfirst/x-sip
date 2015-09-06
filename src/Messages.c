#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parser.h"
#include "Messages.h"
#include "ViaHeader.h"
#include "utils/list/include/list.h"

struct Message {
    struct RequestLine *request;
    t_list *headers;
};

void ExtractHeaderName(char *header, char *name)
{
    char *end = strchr (header, COLON);

    end --;
    while (*end == SPACE) end --;

    strncpy(name, header, end - header + 1);
}

void ParseRequestLine(char *string, struct Message *message)
{
    struct RequestLine *requestLine = CreateRequestLine();

    Parse(string, requestLine, GetRequestLinePattern()); 
    message->request = requestLine;
}

struct Header *MessageGetHeader(const char *name, struct Message *message)
{
    return NULL;
}

void ParseHeader(char *headerString, struct Message *message)
{
    char name[32] = {0};
    
    ExtractHeaderName(headerString, name);
    if (strcmp (name, "Via") == 0) {
        struct ViaHeader *via =  CreateViaHeader();
        struct ParsePattern *viaPattern = GetViaPattern();
        Parse(headerString, via, viaPattern);
        insert_data_at(message->headers, 0, (void*) via);
    }
  
}

int ParseMessage(char *string, struct Message *message)
{
    char *rawMessage = strdup (string);
    char *line = strtok(rawMessage, "\r\n");
    
    ParseRequestLine(line, message);
    line = strtok(NULL, "\r\n");  
    while(line) {
        ParseHeader(line, message);
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
    struct Message *message = (struct Message *)calloc(1,sizeof (struct Message));
    return message;
}

void DestoryMessage (struct Message **message) 
{ 
    if ((*message) != ((void *)0)) {
        DestoryRequestLine((*message)->request);
        free(*message);
        *message = NULL;
    }
}

