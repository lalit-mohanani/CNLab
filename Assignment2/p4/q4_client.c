#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
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

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

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
        //   while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char file_path[BUFFER_SIZE];
            sprintf(file_path, "%s", folder_path, entry->d_name);
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
            // Send file name
            if (send(client_socket, entry->d_name, strlen(entry->d_name), 0)!=strlen(entry->d_name)) {
                perror("Error sending data");
                exit(EXIT_FAILURE);
            }

            FILE *file = fopen(file_path, "rb");
            if (file == NULL) {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }



        // char th[BUFFER_SIZE];
        snprintf(file_path, sizeof(file_path)+2, "%s", folder_path, entry->d_name);

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
        char file_path[BUFFER_SIZE];
        snprintf(file_path, sizeof(file_path), "%s/%s", folder_path, entry->d_name);

        if (stat(file_path, &file_info) != 0) {
            perror("Error getting file info");
            continue;
        }

        if (S_ISREG(file_info.st_mode)) {
            if (pthread_create(&threads[thread_index], NULL, send_file, (void*)file_path) != 0) {
                perror("Error creating thread");
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



// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <arpa/inet.h>
// #include <dirent.h>
// #include <unistd.h>    // close() 

// #define SERVER_IP "127.0.0.1"
// #define SERVER_PORT 12345
// #define BUFFER_SIZE 1024

// void send_folder(int client_socket, const char *folder_path, struct sockaddr_in server_addr) {

//     struct dirent *entry;
//     DIR *dir = opendir(folder_path);

//     if (dir == NULL) {
//         perror("Error opening folder");
//         exit(EXIT_FAILURE);
//     }

//     while ((entry = readdir(dir)) != NULL) {
//         if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
//             char file_path[BUFFER_SIZE];
//             sprintf(file_path, "%s/%s", folder_path, entry->d_name);

//             // Send file name
//             if (sendto(client_socket, entry->d_name, strlen(entry->d_name), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
//                 perror("Error sending data");
//                 exit(EXIT_FAILURE);
//             }

//             FILE *file = fopen(file_path, "rb");
//             if (file == NULL) {
//                 perror("Error opening file");
//                 exit(EXIT_FAILURE);
//             }

//             while (1) {
//                 char buffer[BUFFER_SIZE];
//                 size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, file);

//                 if (bytes_read == 0) {
//                     // End of file
//                     break;
//                 }

//                 if (sendto(client_socket, buffer, bytes_read, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
//                     perror("Error sending data");
//                     exit(EXIT_FAILURE);
//                 }
//             }

//             fclose(file);
//             // Signal the end of the file
//             sendto(client_socket, "", 0, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
//         }
//     }

//     closedir(dir);
//     // Signal the end of the folder
//     sendto(client_socket, "", 0, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
// }

// int main() {
//     int client_socket;
//     struct sockaddr_in server_addr;

//     // Create socket
//     client_socket = socket(AF_INET, SOCK_DGRAM, 0);
//     if (client_socket == -1) {
//         perror("Error creating socket");
//         exit(EXIT_FAILURE);
//     }

//     // Configure server address
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
//     server_addr.sin_port = htons(SERVER_PORT);

//     const char *folder_path = "sh";  // Adjust the folder name as needed

//     send_folder(client_socket, folder_path, server_addr);

//     close(client_socket);

//     printf("Folder sent to %s:%d\n", SERVER_IP, SERVER_PORT);

//     return 0;
// }
