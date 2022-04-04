// Server side implementation of UDP client-server model

#ifdef _WIN32
  /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
  #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501  /* Windows XP. */
  #endif
  #include <winsock2.h>
  #include <Ws2tcpip.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <iostream>
#include <string>
#include <sstream>

#include <ctime>   

#define PORT 5005
#define MAXLINE 1024
class SocketServer{ 
    SocketServer(int port = 5005){ 
        servaddr.sin_family = AF_INET; // IPv4
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(port);
    }
    private:
        struct sockaddr_in servaddr, cliaddr;
    	int sockfd;

};

// Driver code
int main() {
	int sockfd;
	char buffer[MAXLINE];
	char *hello = "Hello from server";
	struct sockaddr_in servaddr, cliaddr;
    #ifdef _WIN32
        WSADATA wsa;

        printf("\nInitialising Winsock...");
        if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
        {
            printf("Failed. Error Code : %d",WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        printf("Initialised.\n");
        
    #endif
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0) {
        #ifdef _WIN32
        printf(WSAGetLastError() + "\n");
        #endif
        exit(EXIT_FAILURE);
    }
	
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
	
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);
	
	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr,
			sizeof(servaddr)) < 0 )
	{
        #ifdef _WIN32
		printf("Failed, error code of " + WSAGetLastError());
        #else
		perror("bind failed");
        #endif
		exit(EXIT_FAILURE);
	}
	
	int len, n;

	len = sizeof(cliaddr); //len is value/resuslt
    while(true){
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                    0, ( struct sockaddr *) &cliaddr,
                    (socklen_t* ) &len);
        buffer[n] = '\0';
        printf("Client : %s\n", buffer);
		auto time = std::chrono::steady_clock::now();
		std::cout << std::stof(buffer) << std::endl;
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time).count() << std::endl;
    }

	
	return 0;
}
