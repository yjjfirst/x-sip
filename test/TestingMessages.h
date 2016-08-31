#define  BINDING_TRYING_MESSAGE (char *)"\
SIP/2.0 100 Trying\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5060\r\n\
From: <sip:88001@192.168.10.62>;tag=1225432999\r\n\
To: <sip:88001@192.168.10.62>;tag=as1d07559a\r\n\
Call-ID: 1222971951\r\n\
CSeq: 1 REGISTER\r\n\
Expires:3600\r\n\
Contact: <sip:88001@192.168.10.1;line=f2fd53ebfa7728f>;expires=3600\r\n\
Content-Length: 0\r\n"

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

#define REMOVE_BINDING_OK_MESSAGE (char *)"\
SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5060\r\n\
From: <sip:88001@192.168.10.62>;tag=1225432999\r\n\
To: <sip:88001@192.168.10.62>;tag=as1d07559a\r\n\
Call-ID: 1222971951\r\n\
CSeq: 1 REGISTER\r\n\
Expires: 0\r\n\
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

#define INVITE_200OK_MESSAGE (char *)"\
SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5061\r\n\
From: <sip:88001@192.168.10.62>;tag=1069855717\r\n\
To: <sip:88002@192.168.10.62>;tag=as6151ad25\r\n\
Call-ID: 97295390\r\n\
CSeq: 20 INVITE\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Content-Type: application/sdp\r\n\
Content-Length: 289\r\n"

#define BYE_200OK_MESSAGE (char *)"\
SIP/2.0 200 OK\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5061\r\n\
From: <sip:88001@192.168.10.62>;tag=1069855717\r\n\
To: <sip:88002@192.168.10.62>;tag=as6151ad25\r\n\
Call-ID: 97295390\r\n\
CSeq: 20 BYE\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Content-Type: application/sdp\r\n\
Content-Length: 289\r\n"

#define INVITE_100TRYING_MESSAGE (char *)"\
SIP/2.0 100 Trying\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5061\r\n\
From: <sip:88001@192.168.10.62>;tag=1226271270\r\n\
To: <sip:88002@192.168.10.62>\r\n\
Call-ID: 778885328\r\n\
CSeq: 20 INVITE\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Content-Length: 0\r\n"

#define INVITE_180RINGING_MESSAGE (char *)"\
SIP/2.0 180 Ringing\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;branch=z9hG4bK1491280923;received=192.168.10.1;rport=5061\r\n\
From: <sip:88001@192.168.10.62>;tag=1226271270\r\n\
To: <sip:88002@192.168.10.62>;tag=as5cde26a4\r\n\
Call-ID: 778885328\r\n\
CSeq: 20 INVITE\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Content-Length: 0\r\n"

#define INCOMMING_INVITE_MESSAGE (char *)"\
INVITE sip:88001@192.168.10.1:5061;line=987565ffceb68b1 SIP/2.0\r\n\
Via: SIP/2.0/UDP 192.168.10.62:5060;branch=z9hG4bK27dc30b4;rport\r\n\
Max-Forwards: 70\r\n\
From: \"88002\" <sip:88002@192.168.10.62>;tag=as47cc7458\r\n\
To: <sip:88001@192.168.10.1:5061;line=987565ffceb68b1>\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Call-ID: 00ef25c213336598033d4bf8248deff7@192.168.10.62:5060\r\n\
CSeq: 102 INVITE\r\n\
Content-Length: 284\r\n"

#define INCOMMING_INVITE_MESSAGE_WITH_TO_TAG (char *)"\
INVITE sip:88001@192.168.10.1:5061;line=987565ffceb68b1 SIP/2.0\r\n\
Via: SIP/2.0/UDP 192.168.10.62:5060;branch=z9hG4bK27dc30b4;rport\r\n\
Max-Forwards: 70\r\n\
From: \"88002\" <sip:88002@192.168.10.62>;tag=as47cc7458\r\n\
To: <sip:88001@192.168.10.1:5061;line=987565ffceb68b1>;tag=ad47cc9876\r\n\
Contact: <sip:88002@192.168.10.62:5060>\r\n\
Call-ID: 00ef25c213336598033d4bf8248deff7@192.168.10.62:5060\r\n\
CSeq: 102 INVITE\r\n\
Content-Length: 284\r\n"

#define BYE_MESSAGE (char *)"\
BYE sip:88001@192.168.10.62:5060 SIP/2.0\r\n\
CSeq: 1 BYE\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5060;branch=z9hG4bK56fb2ea6-fe10-e611-972d-60eb69bfc4e8;rport;send-by=192.168.10.101\r\n\
User-Agent: Ekiga/4.0.1\r\n\
From: <sip:88002@192.168.10.1>;tag=2eb78b83-fe10-e611-972d-60eb69bfc4e8\r\n\
Call-ID: 32ee1a624979941474ca0eab6d2d2b37@192.168.10.62:5060\r\n\
To: \"88001\" <sip:88001@192.168.10.62>;tag=as5c1841ad\r\n\
Contact: \"Martin\" <sip:88002@192.168.10.1:5060>\r\n\
Content-Length: 0\r\n\
Max-Forwards: 70\r\n"

#define ACK_MESSAGE "\
ACK sip:3401@10.6.2.10:5060 SIP/2.0\r\n\
Via: SIP/2.0/UDP  10.6.3.1:5060;branch=z9hG4bKB1C57\r\n\
From: <sip:4105553501@10.6.3.1>;tag=105741C-1D5E\r\n\
To: <sip:3401@10.6.2.10>;tag=16777231\r\n\
Date: Fri, 06 Jan 2006 5:35:13 GMT\r\n\
Call-ID: E937365B-2C0C11D6-802FA93D-4772A3BB@10.6.3.1\r\n\
Max-Forwards: 70\r\n\
CSeq: 101 ACK\r\n\
Content-Length: 0\r\n"

#define MESSAGE_WITH_WRONG_HEADER "\
ACK sip:3401@10.6.2.10:5060 SIP/2.0\r\n\
Via: SIP/2.0/UDP  10.6.3.1:5060;branch=z9hG4bKB1C57\r\n\
From: <sip:4105553501@10.6.3.1>;tag=105741C-1D5E\r\n\
ASDFASDFa\r\n\
:\r\n\
::::\r\n\
To: <sip:3401@10.6.2.10>;tag=16777231\r\n\
Date: Fri, 06 Jan 2006 5:35:13 GMT\r\n\
Call-ID: E937365B-2C0C11D6-802FA93D-4772A3BB@10.6.3.1\r\n\
Max-Forwards: 70\r\n\
CSeq: 101 ACK\r\n\
Content-Length: 0\r\n"

#define MESSAGE_WITH_LONG_HEADER_NAME "\
ACK sip:3401@10.6.2.10:5060 SIP/2.0\r\n\
Via: SIP/2.0/UDP  10.6.3.1:5060;branch=z9hG4bKB1C57\r\n\
From: <sip:4105553501@10.6.3.1>;tag=105741C-1D5E\r\n\
ASDFASDFaasdfasfsadfasdfsadfsadfsadfasdfasdfsadfasdfsadfasdfasfsafsafasfsafsaf:\r\n\
To: <sip:3401@10.6.2.10>;tag=16777231\r\n\
Date: Fri, 06 Jan 2006 5:35:13 GMT\r\n\
Call-ID: E937365B-2C0C11D6-802FA93D-4772A3BB@10.6.3.1\r\n\
Max-Forwards: 70\r\n\
CSeq: 101 ACK\r\n\
Content-Length: 0\r\n"

#define MESSAGE_WITH_CONTENT "\
INVITE sip:88002@192.168.10.62 SIP/2.0\r\n\
Via: SIP/2.0/UDP 192.168.10.1:5061;rport;branch=z9hG4bK826287637\r\n\
From: <sip:88001@192.168.10.62>;tag=701705428\r\n\
To: <sip:88002@192.168.10.62>\r\n\
Call-ID: 955839666\r\n\
CSeq: 20 INVITE\r\n\
Contact: <sip:martin@(null)>\r\n\
Content-Type: application/sdp\r\n\
Allow: INVITE, ACK, CANCEL, OPTIONS, BYE, REFER, NOTIFY, MESSAGE, SUBSCRIBE, INFO\r\n\
Max-Forwards: 70\r\n\
User-Agent: Linphone/3.6.1 (eXosip2/4.0.0)\r\n\
Subject: Phone call\r\n\
Content-Length:   435\r\n\
\r\n\
v=0\r\n\
o=88001 686 894 IN IP4 192.168.10.1\r\n\
s=Talk\r\n\
c=IN IP4 192.168.10.1\r\n\
t=0 0\r\n\
m=audio 7078 RTP/AVP 124 111 110 0 8 101\r\n\
a=rtpmap:124 opus/48000\r\n\
a=fmtp:124 useinbandfec=1; usedtx=1\r\n\
a=rtpmap:111 speex/16000\r\n\
a=fmtp:111 vbr=on\r\n\
a=rtpmap:110 speex/8000\r\n\
a=fmtp:110 vbr=no\r\n\
a=rtpmap:101 telephone-event/8000\r\n\
a=fmtp:101 0-11\r\n\
m=video 9078 RTP/AVP 103 99\r\n\
a=rtpmap:103 VP8/90000\r\n\
a=rtpmap:99 MP4V-ES/90000\r\n\
a=fmtp:99 profile-level-id=3\r\n"
