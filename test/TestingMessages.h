#define ADD_BINDING_OK_MESSAGE (char *)"\
SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5060\r\n\
From: <sip:88001@192.168.10.62>;tag=1225432999\r\n\
To: <sip:88001@192.168.10.62>;tag=as1d07559a\r\n\
Call-ID: 1222971951\r\n\
CSeq: 1 REGISTER\r\n\
Expires: 3600\r\n\
Contact: <sip:88001@192.168.10.1;line=f2fd53ebfa7728f>;expires=3600\r\n\
Content-Length: 0\r\n"

#define UNAUTHORIZED_MESSAGE  (char *)"\
SIP/2.0 401 Unauthorized\r\n                                            \
Via: SIP/2.0/UDP 192.168.10.1:5064;branch=z9hG4bK1491280923\r\n         \
From: <sip:88001@192.168.10.62>;tag=959424103\r\n                       \
To: <sip:88001@192.168.10.62>;tag=as7a073fad\r\n                        \
Call-ID: 1966073125\r\n                                                 \
CSeq: 1 REGISTER\r\n                                                    \
Server: Asterisk PBX 1.8.12.2\r\n                                       \
Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, SUBSCRIBE, NOTIFY, INFO, PUBLISH\r\n \
Supported: replaces, timer\r\n                                          \
WWW-Authenticate: Digest algorithm=MD5, realm=\"asterisk\", nonce=\"1cd2586e\"\r\n \
Content-Length: 0\r\n\r\n"

#define OK_MESSAGE_RECEIVED (char *)"\
SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5061\r\n \
From: <sip:88001@192.168.10.62>;tag=1069855717\r\n                      \
To: <sip:88002@192.168.10.62>;tag=as6151ad25\r\n                        \
Call-ID: 97295390\r\n                                                   \
CSeq: 20 INVITE\r\n                                                     \
Contact: <sip:88002@192.168.10.62:5060>\r\n                             \
Content-Type: application/sdp\r\n                                       \
Content-Length: 289\r\n"

#define RINGING_MESSAGE_RECEIVED (char *)"\
SIP/2.0 180 Ringing\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5061\r\n\
From: <sip:88001@192.168.10.62>;tag=1226271270\r\n\
To: <sip:88002@192.168.10.62>;tag=as5cde26a4\r\n\
Call-ID: 778885328\r\n\
CSeq: 20 INVITE\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Content-Length: 0\r\n"

#define INCOMMING_INVITE_MESSAGE (char *)"\
INVITE sip:88001@192.168.10.1:5061;line=987565ffceb68b1 SIP/2.0\r\n     \
Via: SIP/2.0/UDP 192.168.10.62:5060;branch=z9hG4bK27dc30b4;rport\r\n    \
Max-Forwards: 70\r\n                                                    \
From: \"88002\" <sip:88002@192.168.10.62>;tag=as47cc7458\r\n            \
To: <sip:88001@192.168.10.1:5061;line=987565ffceb68b1>\r\n              \
Contact: <sip:88002@192.168.10.62:5060>\r\n                             \
Call-ID: 00ef25c213336598033d4bf8248deff7@192.168.10.62:5060\r\n        \
CSeq: 102 INVITE\r\n                                                    \
Content-Length: 284\r\n"

#define INCOMMING_CANCEL_MESSAGE (char *)"\
CANCEL sip:88001@192.168.10.1:5061;line=987565ffceb68b1 SIP/2.0\r\n     \
Via: SIP/2.0/UDP 192.168.10.62:5060;branch=z9hG4bK27dc30b4;rport\r\n    \
Max-Forwards: 70\r\n                                                    \
From: \"88002\" <sip:88002@192.168.10.62>;tag=as47cc7458\r\n            \
To: <sip:88001@192.168.10.1:5061;line=987565ffceb68b1>\r\n              \
Contact: <sip:88002@192.168.10.62:5060>\r\n                             \
Call-ID: 00ef25c213336598033d4bf8248deff7@192.168.10.62:5060\r\n        \
CSeq: 102 CANCEL\r\n                                                    \
Content-Length: 0\r\n"

#define  INCOMMING_BYE_MESSAGE (char *) "\
BYE sip:88001@192.168.10.62:5060 SIP/2.0\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5064;rport;branch=z9hG4bK573969419\r\n    \
From: <sip:88004@192.168.10.62>;tag=2074940689\r\n                      \
To: <sip:88001@192.168.10.62>;tag=as317b5f26\r\n                        \
Call-ID: 1312549293\r\n                                                 \
CSeq: 22 BYE\r\n                                                        \
Contact: <sip:88004@192.168.10.1:5064>\r\n                              \
Max-Forwards: 70\r\n                                                    \
User-Agent: Linphone/3.6.1 (eXosip2/4.1.0)\r\n                          \
Content-Length: 0\r\n\r\n"
