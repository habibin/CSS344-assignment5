#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define RED "\e[0;31m"  //color red is defined

// Error function used for reporting issues
void error(const char *msg) {
perror(msg);
exit(1);
} 

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, int portNumber){

// Clear out the address struct
memset((char*) address, '\0', sizeof(*address)); 

// The address should be network capable
address->sin_family = AF_INET;
// Store the port number
address->sin_port = htons(portNumber);
// Allow a client at any address to connect to this server
address->sin_addr.s_addr = INADDR_ANY;
}



int main(int argc, char *argv[]){
int connectionSocket;

struct sockaddr_in serverAddress, clientAddress;
socklen_t sizeOfClientInfo = sizeof(clientAddress);


// Create the socket that will listen for connections
int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
if (listenSocket < 0) {
    fprintf(stderr, RED "ERROR opening socket");
}

// Set up the address struct for the server socket
setupAddressStruct(&serverAddress, atoi(argv[1]));

// Associate the socket to the port
int binding_status = bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
if (binding_status < 0){
    fprintf(stderr, RED "ERROR on binding");
    exit(1);
}

// Start listening for connetions. Allow up to 5 connections to queue up
listen(listenSocket, 5); 

// Accept a connection, blocking if one is not available until one connects
while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
        error("ERROR on binding");
    }

    printf("SERVER: Connected to client running at host %d port %d\n", 
                        ntohs(clientAddress.sin_addr.s_addr),
                        ntohs(clientAddress.sin_port));

   // Get the handshake message from the client and display it
    char handshake_buf[1];
    // Read the client's message from the socket
    char charsRead = recv(connectionSocket, handshake_buf, 1, 0); 
    if (charsRead < 0){
      fprintf(stderr,"ERROR reading from socket");
    }
    printf("SERVER: I received this from the client: \"%s\"\n", handshake_buf);

    // Close the connection socket for this client
    close(connectionSocket); 
}
    
    // Close the listening socket
    close(listenSocket); 

    return 0;
}