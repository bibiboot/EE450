#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

char PORT[100] ;
char HOST[100];
char WWWROOT[100];
char *CONFIG_FILE = "httpd.conf";

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

        int new_sock = accept(skid, res->ai_addr, &(res->ai_addrlen));

        send(new_sock, "Welcome new client\n", 1000, 0);

        close(new_sock);

    }

    return 0;
}

int main(int argc, char *argv[]){
    init();

    start();

    print_init();
}
