#include <stdio.h>
#include <string.h>

char *get_header(char *header_string, char *target_header);
void get_status_line(char *header_string, char *htype, char *url, char *version);

int main(int argc, char *argv[]){
    //char *sample = "Connection: Close\n Content-type: text_html\n Content-length: 1000";
    char *sample = "GET http://www.google.com HTTP\nConnection:Close";
    char dest[1000];
    strcpy(dest, sample);

    //char *result_value = get_header(dest, "Content-type");

    char htype[100], url[100], version[100];
    char *result_value = get_status_line(dest, htype, url, version);
    printf("%s\n", htype);

    return 0;
}

char *get_header(char *header_string, char *target_header){
    char key[100], value[100];
    char *token, *inner_tokken;

    char *temp = strdup(header_string);

    while((token=strsep(&temp, "\n"))!=NULL){
        sscanf(token, "%[^:]:%s", key, value);
        printf("%s\n", value); 
        /*
        char *delta = strdup(token);
        while((inner_tokken=strsep(&delta, ":"))!=NULL){
             
            printf("%s\n", inner_tokken);

        }*/
    }

}

void get_status_line(char *header_string, char *htype, char *url, char *version){
    char *token;
    char *temp = strdup(header_string);

    while((token=strsep(&temp, "\n"))!=NULL){
        sscanf(token, "%[^ ] %[^ ] %s", htype, url, version);
        break;
    }
}

