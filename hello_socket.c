#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[]){

   struct addrinfo hints, *res, *p;
   struct sockaddr_in their;
   void *addr;
   int new_fd;
   char human_addr[100];

   // Initilaize the memory
   memset(&hints, 0, sizeof hints);
   // IPV4 Type
   hints.ai_family =AF_INET;
   // TCP Connection
   hints.ai_socktype = SOCK_STREAM; 

   // Get current host name binded in structure
   getaddrinfo(argv[1], "3000", &hints, &res);

   // Create a socket using the addr structure type
   int fsock = socket(res->ai_family, res->ai_socktype,0);

   // Bind the socket to the above adress structure
   bind(fsock, res->ai_addr, res->ai_addrlen);

   // Free the adress structure as not needed.
   freeaddrinfo(res);
   
   // Make the socket the listen type
   listen(fsock, 10);

   // Loop looking for the incoming connectoin
   while(1){
     printf("Waiting for connection.....\n");
     // their is the address structure to store the connectors details.
     socklen_t their_len = sizeof their;

     // Wait for incoming connection. 
     new_fd = accept(fsock, (struct sockaddr *)&their, &their_len);

     // Send the data on the same connection.
     send(new_fd, "Hello I am the server\n", 100, 0); 

     // Convert the connector details to presentable type format.
     inet_ntop(their.sin_family, &(their.sin_addr), human_addr, 100); 
     printf("The connection is from %s\n", human_addr);

     // CLose the newly created socket 
     close(new_fd);
   }

   return 0;
}
