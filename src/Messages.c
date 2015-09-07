#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parser.h"
#include "Messages.h"
#include "ViaHeader.h"
#include "MaxForwards.h"
#include "utils/list/include/list.h"
#include "Header.h"

struct Message {
    struct RequestLine *request;
    t_list *headers;
};

void ExtractHeaderName(char *header, char *name)
{
    char *end = strchr (header, COLON);

    end --;
    while (*end == SPACE) end --;
    while (*header == SPACE) header ++;

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
    int length = get_list_len(message->headers);
    int i = 0;
    struct Header *header = NULL;

    for (i = 0 ; i < length; i++) {        
        header = (struct Header *) get_data_at(message->headers, i);
        if (strcmp (name, header->name) == 0) {
            break;
        }
    }

    return header;
}

void ParseHeader(char *headerString, struct Message *message)
{
    char name[32] = {0};
    
    ExtractHeaderName(headerString, name);

    if (strcmp (name, "Via") == 0) {
        struct ViaHeader *via =  CreateViaHeader();
        struct ParsePattern *viaPattern = GetViaPattern();
        Parse(headerString, via, viaPattern);
        put_in_list(&message->headers, (void*) via);
    } 
    else if (strcmp(name, "Max-Forwards") == 0) {
        struct MaxForwardsHeader *maxForwards = CreateMaxForwardsHeader();
        struct ParsePattern *maxForwardsPattern = GetMaxForwardsPattern();
        Parse(headerString, maxForwards, maxForwardsPattern);
        put_in_list(&message->headers, (void *) maxForwards);
    }
}

int ParseMessage(char *string, struct Message *message)
{
    char *line = strtok(string, "\r\n");
    
    ParseRequestLine(line, message);
    line = strtok(NULL, "\r\n");
    while(line) {
        ParseHeader(line, message);
        line = strtok(NULL, "\r\n");
    }
    
    return 0;
}

struct RequestLine *MessageGetRequest(struct Message *message)
{
    return message->request;
}

struct Message *CreateMessage () 
{ 
    struct Message *message = (struct Message *)calloc(1,sizeof (struct Message));
    put_in_list (&message->headers, "");
    return message;
}

void MessageDestoryHeaders(struct Message *message)
{
    int length = get_list_len(message->headers);
    int i = 0;
    struct Header *header = NULL;

    for (i = 0 ; i < length; i++) {        
        header = (struct Header *) get_data_at(message->headers, i);
        if (strcmp (header->name, "Via") == 0) {
            DestoryViaHeader((struct ViaHeader *) header);
        }
        else if (strcmp (header->name, "Max-Forwards") == 0) {
            DestoryMaxForwardsHeader((struct MaxForwardsHeader *)header);
        }
    }
}

void DestoryMessage (struct Message **message) 
{ 
    if ((*message) != ((void *)0)) {
        DestoryRequestLine((*message)->request);
        MessageDestoryHeaders(*message);
        destroy_list(&(*message)->headers, NULL);
        free(*message);
        *message = NULL;
    }
}

