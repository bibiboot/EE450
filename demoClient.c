#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char *argv[]){

    struct addrinfo hints, *res;
    char buff[9000];
    memset(&hints, 0, 100);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    getaddrinfo("127.0.0.1", argv[1], &hints, &res);





   while(1){    
    int client_sock = socket(res->ai_family, res->ai_socktype, 0);
    connect(client_sock, res->ai_addr, res->ai_addrlen); 
    if(send(client_sock, "GET /index.html HTTP/1.1\r\n", 100, 0) == -1){
       printf("111\n");
        perror("Send\n");
    }

    if(recv(client_sock, buff, 9000, 0) == -1){
       printf("22\n");
        perror("Recv\n");
    }

    printf("...........................................................................\n");
    printf("%s\n", buff);
    sleep(1);
 
}

    return(0); 
}
