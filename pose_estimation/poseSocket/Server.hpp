// Server side implementation of UDP client-server model

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <string>

#define MAXLINE 1024
class SocketServer{ 
public:
	SocketServer(int);
	std::string recvMessage();
private:
	struct sockaddr_in servaddr, cliaddr;
	int sockfd;
	int port;
	char buffer[MAXLINE];
#ifdef _WIN32
	WSADATA wsa;
#endif
};