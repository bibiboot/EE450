#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>
#include "cache.h"

char PORT[100] ;
char HOST[100];
char WWWROOT[100];
char *CONFIG_FILE = "httpd.conf";

struct sigaction ACT;
sigset_t SET;

int SOCK;

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
    char filename[1000];
    char buff[1000];
    int sockid;
};

int checkHttp(char *httpV, char *req){
    if(strcmp(httpV, "HTTP/1.1")==0 || strcmp(httpV, "HTTP/1.0")==0){
       if(strcmp(req, "GET")==0 || strcmp("HEAD", req)==0){
           return 0;
       }
      
    }
    return -1;
}

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
    // Sets the http-type, Url, version and filename
    char *token;
    char *temp = strdup(header_string);

    while((token=strsep(&temp, "\n"))!=NULL){
        sscanf(token, "%[^ ] %[^ ] %s", htype, url, version);
        strcpy(filename, (strrchr(url, '/') + 1));
        break;
    }
}

int get_request(char *buff, struct header *h){

    // Convert the header in protocl to structure

    get_status_line(buff, h->htype, h->url, h->version, h->filename);

    // Create header structure
    get_header(buff, "Cookie", h->cookie);
    get_header(buff, "Accept-Language", h->accept_lang);

    strcpy(h->path, WWWROOT);
    strcat(h->path, h->url+1);

    return 0;
}

void interrupt_main(){
    //Kill the threads

    printf("Server shutting down\n");
    close(SOCK);
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
    // Print Stats
    printf("........................\n");
    printf("PORT: %s\n", PORT);
    printf("HOST: %s\n", HOST);
    printf("WWWROOT: %s\n", WWWROOT);
    printf("........................\n");
}

int checkMime(char *ext, char *mime){
    FILE *fp = fopen("mime.types", "r");
    char line[1000], key[100], value[100];

    while((fgets(line, 1000, fp))!=NULL){
        if(line[0]=='#') continue;

        sscanf(line, "%[^\t]\t%s", key, value);
        if(strcmp(value, ext)==0) {
            strcpy(mime, key);
            return 0;
        }
    }
    return -1;
}

int get_contentLength(FILE *fp){
    // Get size of the file
    fseek(fp, 0, SEEK_END);
    int contentLength = ftell(fp);
    rewind(fp);
    return contentLength;
}

int extension(char *filename, char *res){
    // Get extention of the file
    strcpy(res, strchr(filename, '.')+1) ;
    return 0;
}

int  sendFile(FILE *fp, int sock){
    // Send the file over socket
    int ch;
    while((ch = fgetc(fp))!=EOF){
        send(sock, &ch, sizeof(char), 0);
    }
    return 0;
}

int sendString(char *message, int sock){
    // Send the string over socket
    send(sock, message, strlen(message), 0);
    return 0;
}

int sendHeader(char *status_code, char *content_type, int totalSize, int sock)
{
        // Create the response

        char *head = "\r\nHTTP/1.1 ";
        char *content_head = "\r\nContent-Type: ";
        char *length_head = "\r\nContent-Length: ";
        char *date_head = "\r\nDate: ";
        char *newline = "\r\n";
        char contentLength[100];
        char message[90000];
        strcpy(message, "");

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

        printf("...................RESPONSE........\n%s", message);
        return 0;
}


void *run_thread(void *data){
    // Main Excecution function

   // Convert data to header data
    struct header *h = (struct header *)data;

    char *mime = (char *)malloc(100);
    memset(mime, '\0', 100);
    char ext[100];

    pthread_t curr_th = pthread_self();
    printf("Thread id = %d\n", (int)curr_th);

    // Parse request and create structure from it.
    if(get_request(h->buff, h)==-1) return;

    if(checkHttp(h->version, h->htype)==-1){
        printf("ERROR: Not known Version\n");
        sendString("501 Not Implemented\n", h->sockid);
        return ;
    }
    
    // Get Extension to check with mime types availabe
    if(extension(h->filename, ext)==-1){
        printf("ERROR: Wrong extension\n");
        sendString("400 Bad Request\n", h->sockid);   
        return ;
    }

    // Get the right mime
    if(checkMime(ext, mime)==-1){
        printf("ERROR: Wrong MIME type\n");
        sendString("400 Bad Request\n", h->sockid);
        return;
    }

    // Get the File 
    FILE *fp = fopen(h->path, "r");
    if(fp==NULL){
        printf("ERROR: Wrong file path\n");
        sendString("404 Not Found\n", h->sockid);
        return ;
    }
    
    // Get content length
    int contentLength = get_contentLength(fp);
    if(contentLength<0) return;

    // Send Header
    if(sendHeader("200 OK", mime, contentLength, h->sockid)==-1) return;

    // Send File
    if(strcmp(h->htype, "GET")==0){
        // Send file only in case of a GET request
        if(sendFile(fp, h->sockid) ==-1) return;
    }

    // Close the File
    while(1);
    close(fp);

}

void run(struct header *h){

    pthread_t th;
    pthread_create(&th, NULL, run_thread, (void*)h);
}

void start(){
    // Create network socket

    struct addrinfo hints, *res;
    char buff[1000];
    
    memset(&hints, 0, 1000);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(HOST, PORT, &hints, &res);

    SOCK = socket(res->ai_family, res->ai_socktype, 0);

    bind(SOCK, res->ai_addr, res->ai_addrlen);

    listen(SOCK, 10);

    while(1){
        unblock_signal();

        printf("...............WAITING...............\n");
        int connect_sock = accept(SOCK, res->ai_addr, &(res->ai_addrlen));
    
        // Recv request
        recv(connect_sock, buff, 1000, 0);

        // Execution thread    
        printf("...............REQUEST...............\n%s", buff);
        struct header *h = (struct header *)malloc(sizeof(*h));
        strcpy(h->buff, buff);
        h->sockid = connect_sock;
        run(h);
        //run_thread(buff);

        // Threads job done, close the socket
        //`close(CONNECT_SOCK);
    }
}

int main(int argc, char *argv[]){
    // Execution starts here
    init();
    print_init();
    block_signal();

    start();
}
