#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int status;
    if((status=fork())==0){
         printf("I am child\n");
    }
    else if(status == -1){
        printf("Error occured\n");
        exit(status);
    }

    printf("Ending\n");
    wait(-1, &status, 0);
    return 0;
}
