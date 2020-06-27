#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

/** server related info **/
#define CCLIENTS 100
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
void talkWithTheClient(int sockfd);

int main() {
    // create a socket for client requests
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sockfd) 
        fatal("Creating server socket failed");
    
    // prepare the address
    sockaddr_in address = {AF_INET, PORT, INADDR_ANY};
    const short size = sizeof(address);
    
    // bind it with the appropriate address
    if( -1 == bind(sockfd, (sockaddr*)&address, size) )
        fatal("failed to name the server socket(bind)");

    // initialize the port(socket descriptor) to listen to at most CCLIENTS clients
    if( -1 == listen(sockfd, CCLIENTS) )
        fatal("Failed to prepare already name server\nsocket for listening");

    pid_t pid = 0;
    int csockfd = 0;
    while(true) {

    // accept request
        csockfd = accept(sockfd, NULL, NULL);
        if(-1 == csockfd ) {
            perror("ERROR");
            continue;
        }

    // fork a child, call TalkWithTheClient() by giving the connects return value as argument
        pid = fork();
        if(-1 == pid) {
            perror("Couldn`t create a process for a client");
            close(csockfd);
            continue;
        }
        else if(0 == pid) {
            close(sockfd); // server main socket
            talkWithTheClient(csockfd);
            close(csockfd); // descriptor of the socket created for a client in child
            return 0;
        }

    // close the new socket(it will be opened in child).
        close(csockfd); // descriptor of the socket created for a client in parent
    }

    return 0;
}

void talkWithTheClient(int sockfd) {
    char message[BUFFSIZE];
    int nProcessed = 0;
    while(true) {
        // getting message from client
        nProcessed = recv(sockfd, message, BUFFSIZE, 0);
        if( nProcessed <= 0 ) break;
        printMessage(message, nProcessed);
        if( endOfChat(message) )
            return;

        //responsing to the client
        nProcessed = getMessage(message);
        nProcessed = send(sockfd, message, nProcessed, 0);
        if( nProcessed <= 0 ) break;
        if( endOfChat(message) )
            return;
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
    std::cout << "client: ";
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
    // stdlib-exit(int) function cloeses open file
    // descriptors befor calling _exit system call
    exit(1);
}
