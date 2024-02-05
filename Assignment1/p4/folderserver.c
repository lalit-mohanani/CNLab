
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

void print_folder_contents(const char* folder_path) {
    DIR* dir = opendir(folder_path);
    if (dir == NULL) {
        perror("Error opening folder");
        exit(EXIT_FAILURE);
    }

    struct dirent* entry;

    printf("Contents of received folder:\n");
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
}


void receive_folder() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", SERVER_PORT);

    // Receive folder name
    ssize_t received_bytes = recvfrom(server_socket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&client_addr, &client_len);
    if (received_bytes == -1) {
        perror("Error receiving folder name");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Null-terminate the received data
    buffer[received_bytes] = '\0';
    printf("Received folder name: %s\n", buffer);
    char* folder_name = buffer;
    rmdir("/home/lalit/CNlab/Assignment1/p4/test");
    if (mkdir("/home/lalit/CNlab/Assignment1/p4/test", 0777) == 0) {
        printf("Directory created successfully.\n");
    } else {
        perror("Failed to create directory");
    }
      while (1) {
        // Receive file name
        if (recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_len) == -1) {
            perror("Error receiving file name");
            break;
        }
        sleep(1);
        if (strcmp(buffer, "EOF") == 0) {
            break;
        }

        char file_name[BUFFER_SIZE];
        sprintf(file_name, "%s/%s", folder_name, buffer);

        // Receive file content
        FILE* file = fopen(file_name, "wb");
        if (!file) {
            perror("Error creating file");
            break;
        }

        while (1) {
            if (recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_len) == -1) {
                perror("Error receiving file content");
                fclose(file);
                break;
            }
            sleep(1);
            if (strcmp(buffer, "EOF") == 0) {
                fclose(file);
                break;
            }
            
            fwrite(buffer, sizeof(char), BUFFER_SIZE, file);
        }
    }
    print_folder_contents(folder_name);
    printf("Folder %s received successfully.\n", folder_name);

    close(server_socket);

    
}

int main() {
    receive_folder();
    return 0;
}
