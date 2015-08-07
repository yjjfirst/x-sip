struct RequestLine {
	char Method[20];
	char Request_URI[64];
	char SIP_Version[16];
};

int parseRequestLine(char *RequestLineString, struct RequestLine *requestLine);

