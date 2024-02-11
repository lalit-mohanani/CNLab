#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <dirent.h>

#define PORT 12345
#define BUF_SIZE 1024

void send_file(int socket, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char buffer[BUF_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(socket, buffer, bytesRead, 0);
    }

    fclose(file);
}

void send_folder(int socket, const char* folderName) {
    DIR* dir = opendir(folderName);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; // Skip "." and ".." directories
        }

        char filepath[BUF_SIZE];
        snprintf(filepath, BUF_SIZE, "%s/%s", folderName, entry->d_name);

        if (entry->d_type == DT_DIR) {
            send_folder(socket, filepath); // Recursively send subdirectories
        } else if (entry->d_type == DT_REG) {
            send_file(socket, filepath); // Send regular files
        }
    }

    closedir(dir);
}

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr;

    // Create UDP socket
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset((char*)&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind to port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // while (1) {
        // Receive folder name from client
        char folderName[BUF_SIZE];
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);

        ssize_t recvSize = recvfrom(serverSocket, folderName, BUF_SIZE, 0, (struct sockaddr*)&clientAddr, &addrLen);
        if (recvSize == -1) {
            perror("recvfrom");
        }

        folderName[recvSize] = '\0'; // Null-terminate the received folder name
        printf("Received folder name: %s\n", folderName);

        // Send folder contents to client
        send_folder(serverSocket, folderName);
    // }

    close(serverSocket);

    return 0;
}
