#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

/** server related info **/
#define PORT 2000
/////// IP is the localhost IP

/** buffers related info **/
#define BUFFSIZE 512

/** end of chat keyword info **/
const char ENDKEY[] = "Goodbye!";

void fatal(const char* msg);
bool endOfChat(const char* msg);
void printMessage(char* message, int size);
int getMessage(char* message);
void talkWithTheServer(int sockfd);

int main() {
    // creating socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sockfd)
        fatal("Creating socket failed");

    // prepare the address
    sockaddr_in address = {AF_INET, PORT, INADDR_ANY};
    const short size = sizeof(address);
    
    // connect to the server
    if( -1 == connect(sockfd, (sockaddr*)&address, size) )
        fatal("Failed to connect to the server");

    talkWithTheServer(sockfd);
    close(sockfd);
    return 0;
}

void talkWithTheServer(int sockfd) {
    char message[BUFFSIZE];
    int nProcessed = 0;
    while(true) {
        // send message to the server
        nProcessed = getMessage(message);
        nProcessed = send(sockfd, message, nProcessed, 0);
        if( nProcessed <= 0 ) break;
        if( endOfChat(message) )
            exit(0);

        // getting message from server
        nProcessed = recv(sockfd, message, BUFFSIZE, 0);
        if( nProcessed <= 0 ) break;
        printMessage(message, nProcessed);
        if( endOfChat(message) )
            exit(0);
    }
    if(-1 == nProcessed)
        fatal("data transferring failed");
}

int getMessage(char* message) {
    std::cout << "me: ";
    std::cin.getline(message, BUFFSIZE);
    return (int)(std::cin.gcount()); // number of bytes read
}

void printMessage(char* message, int size) {
    std::cout << "server: ";
    for(int i = 0; i < size; ++i)
        std::cout << message[i];
    std::cout << std::endl;
}

bool endOfChat(const char* msg) {
    for(int i = 0; ENDKEY[i] != '\0'; ++i)
        if(ENDKEY[i] != msg[i])
            return false;
    return true;
}

void fatal(const char* msg) {
    perror(msg);
    exit(1);
}
