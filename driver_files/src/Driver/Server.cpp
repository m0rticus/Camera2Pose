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

// function that takes in a std::string of data and returns a 2 dimensional list of landmark coordinates
// important shit for wherever I run this function
// float landmark[33][3];
// parse_landmark(buffer, landmark);
int parse_landmark(std::string buffer, float (&landmark)[33][3])
{
	// create stream for easy iteration over words
    std::istringstream iss(buffer);
    // iterate over string, convert each element to a float and stick into landmark
    for (int i = 0; i < 33; i++)
    {
        std::cout << "Landmark: "<< i << "\n";
        for(int j = 0; j < 3; j++)
        {
            std::cout << "Coordinate: "<< j << "\n";
            // read from string if it's not empty, otherwise end
            if(iss)
            {
                // store next stream value in a temp before updating landmark
                std::string temp;
                iss >> temp;
                std::cout << "Temp: " << temp << "\n";
                landmark[i][j] = std::stof(temp);
                std::cout << landmark[i][j] << "\n";
            }
            else
            {
                return 0;
            }
        }
    }

    
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