#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

char PORT[100] ;
char HOST[100];
char WWWROOT[100];
char *CONFIG_FILE = "httpd.conf";

struct sigaction ACT;
sigset_t SET;

void interrupt_main(){

    //Kill the threads
    //pthread_kill(0);
    exit(0);

}

void block_signal(){
    sigemptyset(&SET);
    sigaddset(&SET, SIGINT); 
    pthread_sigmask(SIG_BLOCK, &SET, NULL);
}

void unblock_signal(){
    sigaddset(&SET, SIGINT); 
    pthread_sigmask(SIG_UNBLOCK, &SET, NULL);
 
    ACT. sa_handler = interrupt_main;
    sigaction(SIGINT, &ACT, NULL);
}

void init(){

    FILE *fp = fopen(CONFIG_FILE, "r");
    char line[1000], key[1000], value[1000];

    while(fgets(line, 1000, fp)!=NULL){
        sscanf(line, "%[^ ] %s", key, value);
        if(strcmp(key, "port")==0){
            strcpy(PORT, value);
        }
        else if(strcmp(key, "host")==0){
            strcpy(HOST, value);
        }
        else if(strcmp(key, "wwwroot")==0){
            strcpy(WWWROOT, value);
        }
        else{
            printf("Corrupt httpd.conf\n");
            exit(0);
        }
    }
}

void print_init(){
    printf("PORT: %s\n", PORT);
    printf("HOST: %s\n", HOST);
    printf("WWWROOT: %s\n", WWWROOT);
}

void start(){

    struct addrinfo hints, *res;
    
    memset(&hints, 0, 1000);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(HOST, PORT, &hints, &res);

    int skid = socket(res->ai_family, res->ai_socktype, 0);

    bind(skid, res->ai_addr, res->ai_addrlen);

    listen(skid, 10);

    while(1){
        unblock_signal();

        printf("Waiting for Connection....\n");
        int new_sock = accept(skid, res->ai_addr, &(res->ai_addrlen));
        printf("Connection made....\n");

        if(send(new_sock, "Welcome new client", 100, 0)==-1){
            perror("send");
        }

        close(new_sock);

    }
}



int main(int argc, char *argv[]){
    init();
    print_init();
    block_signal();

    start();

}
