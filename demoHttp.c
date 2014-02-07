#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

int main(int argc, char *argv[]){
    init();

    print_init();
}
