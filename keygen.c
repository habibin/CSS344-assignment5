#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#define RED "\e[1;31m"  //color red is defined

void main(int argc, char* argv[]){

    //checks to make sure an argument is passed
    if( argc < 2){
        fprintf(stderr, RED "[ERROR]: No argument provided ");
    }


    srand(time(0));                                   // sets current time as a seed to generate new numbers
    char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    int key_length = atoi(argv[1]);                   //converts string to number
    int i = 0;
    for (i; i < key_length; i++){
        char new_char =characters[(rand() % 27)];    //generates a random number between 0-27 then converts number to corresponding letter in character array
        fprintf(stdout, "%c",new_char); 
        fflush(stdout);
    }
    fprintf(stdout, "\n");                                   //last character will always be a newline
}
