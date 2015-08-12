struct RequestLine {
	char Method[20];
	char Request_URI[64];
	char SIP_Version[16];
};

int parse(char *header, void *target);
   
