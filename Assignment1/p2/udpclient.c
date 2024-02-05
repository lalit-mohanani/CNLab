#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SERVER_IP "10.10.64.59" //195.35.22.104
// #define SERVER_PORT 12345
#define BUFFER_SIZE 1024
int main() {
    int sockfd;
    struct sockaddr_in serverAddr[5];
    char buffer[BUFFER_SIZE];
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    for(int i=0;i<5;i++){
        memset(&serverAddr[i], 0, sizeof(serverAddr[i]));
        serverAddr[i].sin_family = AF_INET;
        serverAddr[i].sin_addr.s_addr = inet_addr(SERVER_IP);
        serverAddr[i].sin_port = htons(i+3001);
    }
    fgets(buffer, sizeof(buffer), stdin);
    for(int i=0;i<5;i++){
        if (sendto(sockfd, buffer, strlen(buffer), 0,
                   (struct sockaddr*)&serverAddr[i], sizeof(serverAddr[i])) == -1) {
            perror("Error sending message");
            exit(EXIT_FAILURE);
        }
        sleep(1);
    }
    // while (1) {
    //     printf("Enter message: ");
    //     fgets(buffer, sizeof(buffer), stdin);
    //     if (sendto(sockfd, buffer, strlen(buffer), 0,
    //                (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
    //         perror("Error sending message");
    //         exit(EXIT_FAILURE);
    //     }
    //     // ssize_t bytesRead = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
    //     // if (bytesRead == -1) {
    //     //     perror("Error receiving response");
    //     //     exit(EXIT_FAILURE);
    //     // }
    //     // buffer[bytesRead] = '\0';
    //     // printf("Received response from the server: %s\n", buffer);
    //     memset(buffer, 0, sizeof(buffer));
    // }
    close(sockfd);
    return 0;
}
