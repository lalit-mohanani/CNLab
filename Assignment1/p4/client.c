#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUF_SIZE 1024

void receive_file(int socket, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error creating file: %s\n", filename);
        return;
    }

    char buffer[BUF_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = recv(socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytesRead, file);
    }

    fclose(file);
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create UDP socket
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset((char*)&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);// Server IP address

    // Send folder name to server
    char folderName[] = "/home/lalit/CNlab/Assignment1/test";
    sendto(clientSocket, folderName, strlen(folderName), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // Receive files from server
    receive_file(clientSocket, "test"); // Specify the destination folder name

    close(clientSocket);

    return 0;
}
