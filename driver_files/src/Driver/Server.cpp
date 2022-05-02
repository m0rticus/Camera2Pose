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
    memset(&consoleaddr, 0, sizeof(consoleaddr));
    // Bind the socket with the server address

    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
  
    consoleaddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, "127.0.0.1", &consoleaddr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }
    consoleaddr.sin_port = htons(5006);

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
    int max_fd = sockfd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(sockfd, &all_fds);

    struct timeval timeout_val;
    timeout_val.tv_sec = (long int)0;
    timeout_val.tv_usec = 5000;

    if (select(max_fd + 1, &all_fds, NULL, NULL, &timeout_val) < 0) {
        #ifdef _WIN32
        printf("Failed, error code of " + WSAGetLastError());
        #else
        perror("select error");
        #endif
        exit(EXIT_FAILURE);
    }

    if (FD_ISSET(sockfd, &all_fds)) {
        int n;
        int len = sizeof(cliaddr);
        n = recvfrom(sockfd, buffer, MAXLINE,
            0, (struct sockaddr*)&cliaddr,
            (socklen_t*)&len);
        buffer[n] = '\0';
        return buffer;
    }
    return "";
}

void PoseSocketServer::sendMessage(std::string content) {
    char* message = new char[content.size() + 1];
    std::copy(content.begin(), content.end(), message);
    message[content.size()] = '\0';
    int len = sizeof(consoleaddr);
    sendto(sockfd, message, strlen(message), 0, (const struct sockaddr*)&consoleaddr, len);
    delete[] message;
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