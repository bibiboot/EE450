#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>

sigset_t SET;
struct sigaction ACT, ACT1, ACT2;

void loop(int val);
void block_signal();
void unblock_signal();
void interrupt_main(int signum);
void interrupt_thread1(int signum);
void *fn1(void *);
void *fn2(void *);

// Initialize the thread id
pthread_t TH1, TH2;

// Thread data 
struct node {
    int data;
};


int main(int argc, char *argv[]){

    //Create request
    struct node *n = (struct node*)malloc(sizeof(struct node*));
    n->data = 100;

    // Every thread will have the signal blocked
    block_signal();

    // Crated two functions
    pthread_create(&TH1, NULL, fn1, (void *)n);
    pthread_create(&TH2, NULL, fn2, (void *)n);

    // Unblock signal
    unblock_signal();

    // Wait for the threads
    pthread_join(TH1, NULL);
    pthread_join(TH2, NULL);

    printf(" the end\n");
    return (0);
}

void *fn1(void *attr){
    struct node *n = (struct node*)attr;
    printf("The value is %d\n",n->data);

    //Assigning action to SIGUSR1
    ACT1.sa_handler = interrupt_thread1;
    sigaction(SIGUSR1, &ACT1, NULL);
    loop(2);
}

void *fn2(void *attr){
    ACT2.sa_handler = interrupt_thread1;
    sigaction(SIGUSR2, &ACT2, NULL);
    loop(2);
}

void block_signal(){
    sigemptyset(&SET);
    sigaddset(&SET, SIGINT);
    pthread_sigmask(SIG_BLOCK, &SET, NULL);
}

void unblock_signal(){
    // Assigning the main interrupt
    ACT.sa_handler = interrupt_main;
    sigaction(SIGINT, &ACT, NULL);

    // Unblocking the main thread to recieve the interrupt
    pthread_sigmask(SIG_UNBLOCK, &SET, NULL);
}

void interrupt_main(int signum){
    printf("Inside the main interrupt\n");

    pthread_kill(TH1, SIGUSR1);
    pthread_kill(TH2, SIGUSR2);
}

void interrupt_thread1(int signum){
   printf("Killing the thread1\n");
   pthread_exit(0);
}

void loop(int val){
    int i;
    while(1);
}
