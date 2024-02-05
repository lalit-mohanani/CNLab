#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SERVER_IP "195.35.22.104" //195.35.22.104
#define PORT 12345
#define BUFFER_SIZE 1024

void send_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(PORT);

    while (1) {
        size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
        
        if (bytesRead == 0) {
            break;  // End of file
        }
        printf("Sending %ld bytes\n", bytesRead);
        printf("Sending %s\n", buffer);
        if (sendto(sockfd, buffer, bytesRead, 0,
                   (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("Error sending data");
            exit(EXIT_FAILURE);
        }

        sleep(1);  // Optional: add a small delay to control the rate of sending data
    }

    fclose(file);
    close(sockfd);

    printf("File sent successfully: %s\n", filename);
}

int main() {
    send_file("file1.txt");
    // send_file("file2.txt");
    return 0;
}
