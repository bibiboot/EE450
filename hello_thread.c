#include <stdio.h>
#include <pthread.h>

void *test(void *x);

int main(int argc, char *argv[]){

   pthread_t thread1, thread2;
   void *x = (void*)100;
   pthread_create(&thread1, NULL, test, x); 
   pthread_create(&thread2, NULL, test, x); 

   pthread_join(thread1, NULL);
   pthread_join(thread2, NULL);
   return 0;
}

void *test(void *x){
   printf("%d\n", x);
}
