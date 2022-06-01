#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()
#define RED "\e[0;31m"  //color red is defined

/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/


// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(0); 
} 



// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
 
    char* hostname = "localhost";

    // Clear out the address struct
    memset((char*) address, '\0', sizeof(*address)); 

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);

    // Get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname(hostname); 
    if (hostInfo == NULL) { 
        fprintf(stderr, RED "CLIENT: ERROR, no such host\n"); 
        exit(0); 
    }
    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*) &address->sin_addr.s_addr, 
            hostInfo->h_addr_list[0],
            hostInfo->h_length);
}


char* read_func(char* textfile){
    //reads the file and puts it into the buffer
 
    FILE* fp = fopen(textfile, "r"); //opens file

    //error opening file
    if (fp == NULL){
        fprintf(stderr, RED "ERROR in opening file");
    }

    //moves to end of file
    fseek(fp, 0L, SEEK_END);
    //finds filesize
    long int filesize = ftell(fp);
    //Rewinds to beginning of file
    rewind(fp);
    //create a buffer with filesize memory
    char*buffer = (char*)malloc(sizeof(char) * (filesize + 1));
    //reads the file into the buffer
    fread(buffer, sizeof(char), filesize, fp);
    //closes file
    fclose(fp);
    
    return buffer;
}



int main(int argc, char *argv[]) {
    
    struct sockaddr_in serverAddress;

    //sends textfile and keyfile to be read and input into buffer
    char* text_buffer = read_func(argv[1]);
    char* key_buffer = read_func(argv[2]);

    //checks if keyfile is smaller than textfile
    if (strlen(text_buffer) > strlen(key_buffer)){
        fprintf(stderr, RED "ERROR: keyfile is shorter than textfile");
        exit(1);
    }

    // Create a socket
    int socketFD = socket(AF_INET, SOCK_STREAM, 0); 
    if (socketFD < 0){
        fprintf(stderr, RED "CLIENT: ERROR opening socket");
    }

    // Set up the server address struct
    setupAddressStruct(&serverAddress, atoi(argv[3]));

    // Connect to server
    int connection_status = connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (connection_status < 0){
        fprintf(stderr, "Error: cannot make connection to socket");
    }

    // Send message to server to identify itself
    char message[1] = "$";
    char handshake = send(socketFD, message, strlen(message), 0); 
    if (handshake < 0){
        fprintf(stderr, "CLIENT: ERROR writing to socket");
    }
    if (handshake < strlen(message)){
        printf("CLIENT: WARNING: Not all data written to socket!\n");
    }


    // Close the socket
    close(socketFD); 
    return 0;
    
}