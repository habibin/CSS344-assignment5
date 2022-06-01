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
    char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
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
    //verifies each character then puts it into the buffer
    int i = 0;
    while (i < filesize){
        buffer[i] = fgetc(fp);
        if (buffer[i] != '\n' && strchr(characters, buffer[i]) == NULL){
            fprintf(stderr, RED "unapproved character is found in file");
            exit(1);
        }
        i++;
    }
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


    //converts text & key filessizes from number to string
    char num_str1[10];
    char num_str2[10];
    sprintf(num_str1,"%d", strlen(text_buffer));
    sprintf(num_str2,"%d", strlen(key_buffer));

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
    int handshake = send(socketFD, "?\0", 2, 0); 
    if (handshake < 0){
        fprintf(stderr, "CLIENT: ERROR writing to socket");
    }

    // Get handshake message from server
    char handshake_buff[3];
    int charsRead = recv(socketFD, handshake_buff, 2, 0); 
    if (charsRead < 0){
        error("CLIENT: ERROR reading from socket");
    }
    printf("CLIENT: I received this from the server: %s\n", handshake_buff);

    //verifies handshake message
    //if message matches, continue
    if (strcmp(handshake_buff,"@\0") == 0){
        //sends the file size of textfile to server
        int filesize1 = send(socketFD, num_str1 , strlen(num_str1), 0); 
        if (filesize1 < 0){
            fprintf(stderr, "CLIENT: ERROR writing to socket");
        }

       // Get confirmation for filesize from server
        char confirm_buf[3];
        int charsRead1 = recv(socketFD, confirm_buf, 2, 0); 
        if (charsRead1 < 0){
            error("CLIENT: ERROR reading from socket");
        }
        printf("CLIENT: I received this from the server: %s\n", confirm_buf); 

        //sends the textfile to server
        int counter = 0; // keeps track of how many bytes have been sent
        while (1){
            int textfile = send(socketFD, text_buffer , strlen(text_buffer), 0); 
            if (textfile < 0){
                fprintf(stderr, "CLIENT: ERROR writing to socket");
            }
            //checks to see if all file has been sent
            counter = textfile + counter;
            printf("textfile: %d counter: %d strlen: %d\n", textfile,counter, strlen(text_buffer));
            if (counter == strlen(text_buffer)){
                break;
            }
        }
        // //loops until entire file has been sent
        // while(1){
        //     // Sends concatenized text and key file to server
        //     int handshake = send(socketFD, cat_buff , strlen(cat_buff), 0); 
        //     if (handshake < 0){
        //         fprintf(stderr, "CLIENT: ERROR writing to socket");
        //     }
        // }

        close(socketFD);
    }

    //else it closes the socket connection
    else{
        // Close the socket
        close(socketFD); 
    }
    return 0;
    
}