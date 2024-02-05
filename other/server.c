#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Port number not provided. Program terminated\n");
        exit(1);
    }
    int sockfd, newsockfd, portno, n;
    char buffer[255];
    
}