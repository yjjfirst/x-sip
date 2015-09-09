#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Parser.h"
#include "Messages.h"
#include "ViaHeader.h"
#include "MaxForwards.h"
#include "Contacts.h"
#include "utils/list/include/list.h"
#include "Header.h"

struct Message {
    struct RequestLine *request;
    t_list *headers;
};

struct HeaderParser {
    char name[HEADER_NAME_MAX_LENGTH];
    struct Header * (*headerParser)(char *headerString);
    void (*headerDestroyer)(struct Header *header);
};

struct HeaderParser HeaderParsers[] = {
    {HEADER_NAME_VIA, ParseViaHeader, DestoryViaHeader},
    {HEADER_NAME_MAX_FORWARDS, ParseMaxForwardsHeader, DestoryMaxForwardsHeader},
    {HEADER_NAME_TO, ParseContactsHeader, DestoryContactsHeader},
    {HEADER_NAME_FROM, ParseContactsHeader, DestoryContactsHeader},
    {HEADER_NAME_CONTACT, ParseContactsHeader, DestoryContactsHeader},
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
        if (strcmp (name, HeaderGetName(header)) == 0) {
            break;
        }
    }

    return header;
}

void ParseHeader(char *headerString, struct Message *message)
{
    char name[32] = {0};
    int i = 0;
    
    ExtractHeaderName(headerString, name);

    for ( ; i < sizeof(HeaderParsers)/sizeof(struct HeaderParser); i++) {
        if (strcmp (HeaderParsers[i].name , name) == 0)
            put_in_list(&message->headers, (void*) HeaderParsers[i].headerParser(headerString));
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

void DestoryOneHeader(struct Header *header)
{
    int i;

    for (i = 0 ; i < sizeof(HeaderParsers)/sizeof(struct HeaderParser); i++) {
        if (strcmp (HeaderParsers[i].name, header->name) == 0) {
            HeaderParsers[i].headerDestroyer(header);
        }
    }
}

void MessageDestoryHeaders(struct Message *message)
{
    int length = get_list_len(message->headers);
    int i ;
    struct Header *header = NULL;

    for (i = 0 ; i < length; i++) {        
        header = (struct Header *) get_data_at(message->headers, i);
        DestoryOneHeader(header);
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

