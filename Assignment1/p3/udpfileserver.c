#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void receive_file(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

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
    serverAddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    printf("UDP server is listening on port %d...\n", PORT);

    while (1) {
        ssize_t bytesRead = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                     (struct sockaddr*)&clientAddr, &addrLen);

        if (bytesRead == -1) {
            perror("Error receiving data");
            exit(EXIT_FAILURE);
        }

        if (bytesRead == 0) {
            break;  // End of file
        }

        fwrite(buffer, 1, bytesRead, file);
    }

    fclose(file);
    close(sockfd);

    printf("File received successfully: %s\n", filename);
}

int main() {
    receive_file("recieve.txt");  // Specify the desired filename
    return 0;
}
