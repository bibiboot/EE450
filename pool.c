#include <stdio.h>
#include <pthread.h>

void *pool_manager(void *attr);

int WORKERS = 10;
pthread_mutex_t m;
pthread_cond_t cond;

int S[100];
int TOP = -1;

void push(int data){
    if(TOP==99) return;
    S[++TOP] = data;
}

int pop(){
    if(TOP==-1) return -1;
    return S[TOP--];
}


void init_pool(){
    int i; 
    for(i=0;i<WORKERS;i++){
        pthread_t th;
        pthread_create(&th, NULL, pool_manager, NULL);
    }

}

void *pool_manager(void *attr){

    int curr = (int)pthread_self();
    printf("Current th = %d\n", curr);

    while(1){
        
        //Block on the cond until some request in linkedlist
        pthread_mutex_lock(&m);
        if(TOP==-1){
            //Stack is empty then wait
            pthread_cond_wait(&cond, &m);
        }
        int request = pop();
        printf("%d = ThreadID %d\n", request, curr);
        pthread_mutex_unlock(&m);

    }
}


int main(int argc, char *argv[]){
    init_pool();
    sleep(10);
    int i;
    for(i=1;i <=50; i++){
        push(i);
        pthread_cond_broadcast(&cond);
    }
    while(1);
    return 0;
}
