#include <stdio.h>

int main(int argc, char *argv[]){

    int pfd[2];
    pipe(pfd);
    char buf[1024];
    char a[10], b[10];
    int i;

    for (i=0; i<4000; ++i) {

        gettimeofday(&a,0);

        if (fork()>0) {

            write(pfd[1],"+",1); block(); exit(0);

        }

        read(pfd[0],buf,1);

        gettimeofday(&b,0);

        //printf("%llu\n",difference(&a,&b));


    }

}
