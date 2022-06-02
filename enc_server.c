#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include<signal.h>
#include<arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>
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

    // creates variable
    pid_t spawnpid;
    int spawnpidstatus;

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
            fprintf(stderr, RED "ERROR on binding");
            exit(1);
        }

        // creates a child process
	    spawnpid = fork();

        if (spawnpid == -1){
        // Code in this branch will be exected by the parent when fork() fails and the creation of child process fails as well
            perror("fork() failed!");
            exit(1);
        }

        else if (spawnpid == 0){
        // spawnpid is 0. This means the child will execute the code in this branch

            // Get the handshake message from the client
            char handshake_buf[2];
            // Read the client's message from the socket
            int charsRead = recv(connectionSocket, handshake_buf, 2, 0); 
            if (charsRead < 0){
                fprintf(stderr,"ERROR reading from socket");
                exit(1);
            }

            //verifies handshake message
            //if message matches, continue
            if ( strcmp(handshake_buf,"?") == 0){
                // Send a identifying message back to the client
                int charsSent = send(connectionSocket, "@\0", 2, 0); 
                if (charsSent < 0){
                    fprintf(stderr, RED "ERROR writing to socket");
                    exit(1);
                }

                // Get the textfile size from the client
                char buff1[7];
                // Read the client's message from the socket
                int charsRead1 = recv(connectionSocket, buff1, 6, 0); 
                buff1[charsRead1] = '\0';
                if (charsRead1 < 0){
                    fprintf(stderr, RED"ERROR reading from socket");
                    exit(1);
                }

                //Send confirmation for receiving message back to the client
                int charsSent1 = send(connectionSocket, "!\0", 2, 0); 
                if (charsSent1 < 0){
                    fprintf(stderr, RED "ERROR writing to socket");
                    exit(1);
                }

                //converts textfile string size to number
                int text_size = atoi(buff1);
                memset(buff1,0,7);
                //creates textfile buffer with text_size
                char text_buffer[text_size+1];

                //receives textfile from client
                int charsRead2 = recv(connectionSocket, text_buffer, text_size, MSG_WAITALL); 
                if (charsRead2 < 0){
                    fprintf(stderr, RED "ERROR reading from socket");
                    exit(1);
                }
                text_buffer[charsRead2] = '\0';

                //Send confirmation for receiving textfile back to the client
                int charsSent2 = send(connectionSocket, "!\0", 2, 0); 
                if (charsSent2 < 0){
                    fprintf(stderr, RED "ERROR writing to socket");
                    exit(1);
                }




            /*RECEIVING KEYFILE*/
                // Get the keyfile size from the client
                char buff2[7];
                // Read the client's message from the socket
                int charsRead3 = recv(connectionSocket, buff2, 6, 0); 
                buff2[charsRead3] = '\0';
                if (charsRead3 < 0){
                    fprintf(stderr, RED "ERROR reading from socket");
                    exit(1);
                }
                // fprintf(stdout, WHITE "SERVER: I received this from the client: \"%s\"\n", buff2);

                //Send confirmation for receiving key filesize back to the client
                int charsSent3 = send(connectionSocket, "!\0", 2, 0); 
                if (charsSent3 < 0){
                    fprintf(stderr, RED "ERROR writing to socket");
                    exit(1);
                }

                //converts keyfile string size to number
                int key_size = atoi(buff2);
                memset(buff2,0,7);
                //creates keyfile buffer with key_size
                char key_buffer[key_size+1];

                //receives keyfile from client
                int charsRead4 = recv(connectionSocket, key_buffer, key_size, MSG_WAITALL); 
                if (charsRead4 < 0){
                    fprintf(stderr, RED "ERROR reading from socket");
                    exit(1);
                }
                key_buffer[charsRead4] = '\0';

                /*ENCRYPTIAN DONE HERE*/
                char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
                char cipher_buff[text_size+1]; //stores ciphertext after encryptian
                int file_counter = 0; //counter for text and keyfile
                //while loop finds index position and encrypts
                while (text_buffer[file_counter] != '\n'){
                    //finds the textfile index number
                    int text_pos = 0;
                    for(text_pos; text_pos < 27; text_pos++){
                        if(text_buffer[file_counter] == characters[text_pos]){
                            break;
                        }
                    }
                    //finds the keyfile index number
                    int key_pos = 0;
                    for(key_pos; key_pos < 27; key_pos++){
                        if(key_buffer[file_counter] == characters[key_pos]){
                            break;
                        }
                    }

                    //addition occurs
                    int total = text_pos + key_pos;
                    //if number is greater than 25, subtract 26 from it.
                    if(total > 26){
                        total = total-27;
                    }
                    //Converts new number back to character and stores in cipher_buff
                    cipher_buff[file_counter] = characters[total];
                    //increments to the next file index
                    file_counter++;
                }

                cipher_buff[text_size-1] = '\n';
                //Sends cipher_buff to the client
                int charsSent4 = send(connectionSocket, cipher_buff, text_size, 0); 
                if (charsSent4 < 0){
                    fprintf(stderr, RED "ERROR writing to socket");
                    exit(1);
                }

                //Close the connection socket for this client
                close(connectionSocket); 
            }

            //otherwise socket connection is closed
            else{
                fprintf(stderr, "Connected to wrong client\n");
                int charsSent = send(connectionSocket, "@\0", 2, 0); 
                if (charsSent < 0){
                    fprintf(stderr, RED "ERROR writing to socket");
                }
                close(connectionSocket);
            }
        }

        else{
		// The parent process executes this branch
            waitpid(spawnpid, &spawnpidstatus, WNOHANG);
        }
    }
        
    // Close the listening socket
    close(listenSocket); 

    return 0;
}