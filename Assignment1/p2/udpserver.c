#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 1
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    printf("UDP chat server is listening on port %d...\n", SERVER_PORT);

    while (1) {
        // Receive data from a client
        ssize_t bytesRead = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                     (struct sockaddr*)&clientAddr, &addrLen);

        if (bytesRead == -1) {
            perror("Error receiving data");
            exit(EXIT_FAILURE);
        }

        // Null-terminate the received data
        buffer[bytesRead] = '\0';

        // Print received message
        printf("Received message from %s:%d - %s\n",
               inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buffer);

        // Broadcast the message to all clients
        // In a real chat application, you would maintain a list of connected clients and send the message to each of them
    }

    // Close the socket (this should never be reached in this example)
    close(sockfd);

    return 0;
}
