#pragma comment(lib, "Ws2_32.lib")

#include <Driver/Server.hpp>
#include <ctime>   
#include <chrono>

#define MAXLINE 1024

PoseSocketServer::PoseSocketServer(int aPort = 5005): port(aPort){ 
    #ifdef _WIN32
        printf("\nInitialising Winsock...");
        if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
        {
            printf("Failed. Error Code : %d",WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        printf("Initialised.\n");
    #endif
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0) {
        #ifdef _WIN32
        printf(WSAGetLastError() + "\n");
        #endif
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    // Bind the socket with the server address

    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

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
}

std::string PoseSocketServer::recvMessage(){ 
    int n;
    int len = sizeof(cliaddr);
    n = recvfrom(sockfd, buffer, MAXLINE,
                0, ( struct sockaddr *) &cliaddr,
                (socklen_t* ) &len);
    buffer[n] = '\0';
    return buffer;
}

/*
// Driver code
int main() {
	SocketServer s = SocketServer(); 
    while(true){
        std::string buffer = s.recvMessage(); 
        std::cout << "Client : " << buffer << std::endl;
		auto time = std::chrono::steady_clock::now();
		std::cout << std::stof(buffer) << std::endl;
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - time).count() << std::endl;
        float landmark[33][3];
        parse_landmark(buffer, landmark);
	}

	  	
	return 0;
}
*/