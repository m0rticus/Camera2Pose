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
#include <unistd.h>
#include <string.h>

#include <ctime>   

#define PORT 5005
#define MAXLINE 1024

// Driver code
int main() {
	int sockfd;
	char buffer[MAXLINE];
	char *hello = "Hello from server";
	struct sockaddr_in servaddr, cliaddr;
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0 ) {
		perror("socket creation failed");
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
		perror("bind failed");
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
        sendto(sockfd, (const char *)hello, strlen(hello), 0,(const struct sockaddr *) &cliaddr,
                len);
        printf("Hello message sent.\n");
    }

	
	return 0;
}
