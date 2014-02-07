#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char *argv[]){

    struct addrinfo hints, *res;
    char buff[1000];
    memset(&hints, 0, 100);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    getaddrinfo("127.0.0.1", "7000", &hints, &res);

    int client_sock = socket(res->ai_family, res->ai_socktype, 0);

    connect(client_sock, res->ai_addr, res->ai_addrlen); 

    if(send(client_sock, "I am client\n", 100, 0) == -1){
        perror("Send\n");
    }

    if(recv(client_sock, buff, 1000, 0) == -1){
        perror("Recv\n");
    }
    printf("XXX\n");
    printf("%s\n", buff);

    return(0); 
}
