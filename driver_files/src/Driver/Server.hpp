// Server side implementation of UDP client-server model
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <string>

#define MAXLINE 1024

class PoseSocketServer{ 
public:
	PoseSocketServer(int);
	std::string recvMessage();
	void sendMessage(std::string);
private:
	struct sockaddr_in servaddr, cliaddr, consoleaddr;
	int sockfd;
	int port;
	char buffer[MAXLINE];
#ifdef _WIN32
	WSADATA wsa;
#endif
};