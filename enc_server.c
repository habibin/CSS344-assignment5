#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define RED "\e[0;31m"  //color red is definedI
#define WHITE "\e[0;37m"

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

        fprintf(stdout, WHITE "SERVER: Connected to client running at host %d port %d\n", 
                            ntohs(clientAddress.sin_addr.s_addr),
                            ntohs(clientAddress.sin_port));

    // Get the handshake message from the client
        char handshake_buf[2];
        // Read the client's message from the socket
        int charsRead = recv(connectionSocket, handshake_buf, 2, 0); 
        if (charsRead < 0){
            fprintf(stderr,"ERROR reading from socket");
        }
        fprintf(stdout, WHITE "SERVER: I received this from the client: \"%s\"\n", handshake_buf);

        //verifies handshake message
        //if messagematch, continue
        if ( strcmp(handshake_buf,"?") == 0){
            // Send a identifying message back to the client
            int charsSent = send(connectionSocket, "@\0", 2, 0); 
            if (charsSent < 0){
                error("ERROR writing to socket");
            }

            // Get the textfile size from the client
            char buff1[7];
            // Read the client's message from the socket
            int charsRead1 = recv(connectionSocket, buff1, 6, 0); 
            buff1[charsRead1] = '\0';
            if (charsRead1 < 0){
                fprintf(stderr,"ERROR reading from socket");
            }
            fprintf(stdout, WHITE "SERVER: I received this from the client: \"%s\"\n", buff1);

            // Send confirmation for receiving message back to the client
            int charsSent1 = send(connectionSocket, "!\0", 2, 0); 
            if (charsSent1 < 0){
                error("ERROR writing to socket");
            }

            //converts textfile string size to number
            int text_size = atoi(buff1);
            memset(buff1,0,7);
            //creates textfile buffer with text_size
            char text_buffer[text_size+1];

            //receives textfile from client
            int charsRead2 = recv(connectionSocket, text_buffer, text_size, MSG_WAITALL); 
            if (charsRead2 < 0){
                fprintf(stderr,"ERROR reading from socket");
            }
            text_buffer[charsRead2] = '\0';
            fprintf(stdout, WHITE "SERVER: I received this from the client: \"%s\"\n", text_buffer);

            // Close the connection socket for this client
            close(connectionSocket); 
        }

        //otherwise socket connection is closed
        else{
            fprintf(stderr, RED "Connected to wrong client\n");
            close(connectionSocket);
        }
    }
        
        // Close the listening socket
        close(listenSocket); 

        return 0;
}