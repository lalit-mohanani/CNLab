#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/stat.h>

#define MAX_MSG_SIZE 1024
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 12345

void* send_file(void* arg) {
    char* filepath = (char*)arg;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error in socket creation");
        pthread_exit(NULL);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in connecting to server");
        close(client_socket);
        pthread_exit(NULL);
    }

    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("Error opening file");
        close(client_socket);
        pthread_exit(NULL);
    }
    printf("Sending file: %s\n", filepath);

    char buffer[MAX_MSG_SIZE];
    size_t bytes_read;
    send(client_socket,filepath, sizeof(filepath), 0);
    sleep(1);
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(client_socket, buffer, bytes_read, 0) != bytes_read) {
            perror("Error sending file");
            fclose(file);
            close(client_socket);
            pthread_exit(NULL);
        }
    }

    fclose(file);
    close(client_socket);
    pthread_exit(NULL);
}

void send_folder_parallel(const char* folder_path) {
    DIR* dir;
    struct dirent* entry;

    dir = opendir(folder_path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct stat file_info;
    int num_files = 0;
    while ((entry = readdir(dir)) != NULL) {
        char file_path[MAX_MSG_SIZE];
        snprintf(file_path, sizeof(file_path), "%s/%s", folder_path, entry->d_name);

        if (stat(file_path, &file_info) != 0) {
            perror("Error getting file info");
            continue;
        }

        if (S_ISREG(file_info.st_mode)) {
            num_files++;
        }
    }

    rewinddir(dir);
    printf("Number of files in the folder: %d\n", num_files);

    pthread_t threads[num_files];
    int thread_index = 0;

    while ((entry = readdir(dir)) != NULL) {
        char file_path[MAX_MSG_SIZE];
        snprintf(file_path, sizeof(file_path), "%s/%s", folder_path, entry->d_name);

        if (stat(file_path, &file_info) != 0) {
            perror("Error getting file info");
            continue;
        }

        if (S_ISREG(file_info.st_mode)) {
            char* thread_file_path = strdup(file_path); // Allocate memory for each thread's file path
            if (thread_file_path == NULL) {
                perror("Error duplicating file path");
                continue;
            }

            if (pthread_create(&threads[thread_index], NULL, send_file, (void*)thread_file_path) != 0) {
                perror("Error creating thread");
                free(thread_file_path); // Free allocated memory in case of failure
                exit(EXIT_FAILURE);
            }
            thread_index++;
        }
    }

    for (int i = 0; i < num_files; i++) {
        pthread_join(threads[i], NULL);
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <folder_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* folder_path = argv[1];

    send_folder_parallel(folder_path);

    printf("Folder sent successfully!\n");

    return 0;
}
