#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int arg, char *argv[]){
    struct addrinfo hints, *res;
    char buf[100];

    // Initialize
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // Fill the address structure for server
    getaddrinfo(argv[1], "3000", &hints, &res);

    // Create the socket with which to connect the server
    int sockfd = socket( res->ai_family, res->ai_socktype, 0);

    struct sockaddr_in *server_addr;
    server_addr = (struct sockaddr_in *)(res->ai_addr);
    char server_human[100];

    // Convert the server details to human readable format
    inet_ntop(res->ai_family, server_addr, server_human, 100);   
    printf("Trying to connec to server: %s\n", server_human);

    // Connect with server
    connect(sockfd, res->ai_addr, res->ai_addrlen);
    
    // Recieved message from the server
    recv(sockfd, buf, 100, 0);
    printf("Recieved message: %s\n", buf);

    return 0;
}
