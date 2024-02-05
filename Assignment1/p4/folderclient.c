#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

void send_folder(const char* folder_path) {
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    // Send folder name
    sendto(client_socket, folder_path, strlen(folder_path), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Traverse the folder and send files
    DIR* dir = opendir(folder_path);
    if (dir == NULL) {
        perror("Error opening folder");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        char file_path[BUFFER_SIZE];
        sprintf(file_path, "%s/%s", folder_path, entry->d_name);

        struct stat file_stat;
        stat(file_path, &file_stat);

        if (S_ISREG(file_stat.st_mode)) {
            // Send file name
            sendto(client_socket, entry->d_name, strlen(entry->d_name), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

            // Send file content
            FILE* file = fopen(file_path, "rb");
            if (file) {
                while (1) {
                    size_t bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, file);
                    if (bytesRead == 0) {
                        break;
                    }

                    sendto(client_socket, buffer, bytesRead, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
                }

                fclose(file);
            }

            // Signal end of file
            sendto(client_socket, "EOF", 3, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
        }
    }
    sendto(client_socket, "EOF", 3, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    closedir(dir);
    close(client_socket);

    printf("Folder %s sent successfully.\n", folder_path);
}

int main() {
    const char* folder_path = "/home/lalit/CNlab/Assignment1/test";
    send_folder(folder_path);
    return 0;
}
