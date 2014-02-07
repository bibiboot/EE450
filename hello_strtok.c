#include <stdio.h>
#include <string.h>

char *get_header(char *header_string, char *target_header);

int main(int argc, char *argv[]){
    char *sample = "Connection: Close\nContent-type: text_html\nContent-length: 1000";
    char dest[1000];
    strcpy(dest, sample);

    char *result_value = get_header(dest, "Content-type");

    //printf("%s\n", result_value);

    return 0;
}

char *get_header(char *header_string, char *target_header){
    char *token = strtok(header_string, "\n");
    char key[100], value[100];

    while(token!=NULL){
        //sscanf(token, "%[^ ] %s", key, value);
        printf("%s\n", token);
        token = strtok(NULL, " ");
    }

}
