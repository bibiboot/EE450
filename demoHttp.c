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

int CONNECT_SOCK;
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
    // Sets the http-type, Url, version and filename
    char *token;
    char *temp = strdup(header_string);

    while((token=strsep(&temp, "\n"))!=NULL){
        sscanf(token, "%[^ ] %[^ ] %s", htype, url, version);
        strcpy(filename, (strrchr(url, '/') + 1));
        break;
    }
}

int get_request(char *buff, struct header **h){

    // Convert the header in protocl to structure

    get_status_line(buff, (*h)->htype, (*h)->url, (*h)->version, (*h)->filename);

    // Create header structure
    char temp[100];
    get_header(buff, "Cookie", temp);
    strcpy((*h)->cookie, temp);
    //get_header(buff, "Accept-Language", temp);
    //strcpy(h->accept_lang, temp);

    strcpy((*h)->path, WWWROOT);
    strcat((*h)->path, (*h)->url+1);

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

int  sendFile(FILE *fp){
    // Send the file over socket
    int ch;
    while((ch = fgetc(fp))!=EOF){
        send(CONNECT_SOCK, &ch, sizeof(char), 0);
    }
    return 0;
}

int sendString(char *message){
    // Send the string over socket
    send(CONNECT_SOCK, message, strlen(message), 0);
    return 0;
}

void sendHeader(char *status_code, char *content_type, int totalSize)
{
        // Create the response

        char *head = "\r\nHTTP/1.1 ";
        char *content_head = "\r\nContent-Type: ";
        char *length_head = "\r\nContent-Length: ";
        char *date_head = "\r\nDate: ";
        char *newline = "\r\n";
        char contentLength[100];
        char message[90000];

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

        sendString(message);

        printf("...................RESPONSE........\n%s", message);
}


void run_thread(char *buff){

    struct header *h = (struct header *)malloc(100*sizeof(struct header *));

    // Main Excecution function
    char ext[100];

    // Parse request and create structure from it.
    get_request(buff, &h);

    // Get the File 
    FILE *fp = fopen(h->path, "r");
    if(fp==NULL) return;
    
    // Get content length
    int contentLength = get_contentLength(fp);

    // Get Extension to check with mime types availabe
    extension(h->filename, ext);

    // Get the right mime
    

    // Send Header
    sendHeader("200 OK", "text/html", contentLength);

    // Send File
    sendFile(fp);

    // Close the File
    close(fp);
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
        CONNECT_SOCK = accept(SOCK, res->ai_addr, &(res->ai_addrlen));
    
        // Recv request
        recv(CONNECT_SOCK, buff, 1000, 0);

        // Execution thread    
        printf("...............REQUEST...............\n%s", buff);
        run_thread(buff);

        // Threads job done, close the socket
        close(CONNECT_SOCK);
    }
}

int main(int argc, char *argv[]){
    // Execution starts here
    init();
    print_init();
    block_signal();

    start();
}
