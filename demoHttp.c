#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>

char PORT[100] ;
char HOST[100];
char WWWROOT[100];
char *CONFIG_FILE = "httpd.conf";

struct sigaction ACT;
sigset_t SET;

struct header {
    char htype[100];
    char url[100];
    char version[100];
    char host[100];
    char connection[100];
    char cache_control[100];
    char accept[100];
    char ua[100];
    char accept_encoding[100];
    char accept_lang[100]; 
    char cookie[100];
    char path[100];
    char *filename;
};

void get_header(char *header_string, char *target_header, char *res){
    char key[100];
    char *token, *inner_tokken;
    char *temp = strdup(header_string);

    while((token=strsep(&temp, "\n"))!=NULL){
        sscanf(token, "%[^:]:%s", key, res);
        if(strcmp(target_header, key)==0) return; 
    }
}

void get_status_line(char *header_string, char *htype, char *url, char *version, char *filename){
    char *token;
    char *temp = strdup(header_string);

    while((token=strsep(&temp, "\n"))!=NULL){
        sscanf(token, "%[^ ] %[^ ] %s", htype, url, version);
        filename = strrchr(url, '/') + 1;
        break;
    }
}

struct header* get_request(char *buff){

    // Convert the header in protocl to structure

    struct header *h = (struct header *)malloc(sizeof(struct header *));
    get_status_line(buff, h->htype, h->url, h->version, h->filename);

    get_header(buff, "Cookie", h->cookie);
    get_header(buff, "Cache-Control", h->cache_control);

    strcpy(h->path, WWWROOT);
    strcat(h->path, h->url+1);

    return h;
}

void interrupt_main(){
    //Kill the threads

    printf("Server shutting down\n");
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

int get_contentLength(FILE *fp){
    fseek(fp, 0, SEEK_END);
    int contentLength = ftell(fp);
    return contentLength;
}

void sendHeader(char *status_code, char *content_type, int totalSize, int sock)
{
        char *head = "\r\nHTTP/1.1 ";
        char *content_head = "\r\nContent-Type: ";
        char *length_head = "\r\nContent-Length: ";
        char *date_head = "\r\nDate: ";
        char *newline = "\r\n";
        char contentLength[100];
        char message[1000];

        time_t rawtime;
        time(&rawtime);

        sprintf(contentLength, "%d", totalSize);

        strcat(message, head);
        strcat(message, status_code);
        strcat(message, content_head);
        strcat(message, content_type);
        strcat(message, length_head);
        strcat(message, contentLength);
        strcat(message, date_head);
        strcat(message, (char *)ctime(&rawtime));
        strcat(message, newline);

        sendString(message, sock);

}


void run_thread(char *buff, int sock){
    // Main Excecution function
    char line[100];

    // Parse request and create structure from it.
    struct header *h = get_request(buff);

    // Get the File 
    FILE *fp = fopen(h->path, "r");
    
    // Get content length
    int contentLength = get_contentLength(fp);

    // Send Header
    sendHeader("200 OK", "text/html", contentLength, sock);

    //Send File
    
}

void start(){
    // Create network socket

    struct addrinfo hints, *res;
    char buff[1000];
    
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
    
        recv(new_sock, buff, 1000, 0);

        if(send(new_sock, "Welcome new client", 100, 0)==-1){
            perror("send");
        }
    
        // Execution thread    
        run_thread(buff, new_sock);

        close(new_sock);

    }
}

int main(int argc, char *argv[]){
    init();
    print_init();
    block_signal();

    start();
}
