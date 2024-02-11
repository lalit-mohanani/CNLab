#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_MSG_SIZE 1024
#define SERVER_PORT 12345

void handle_client(int client_socket, char* folder_path) {
    char buffer[MAX_MSG_SIZE];
    ssize_t bytes_received;

    while(1){
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        FILE *file = fopen(buffer, "wb");
        if (file == NULL) {
            perror("Error creating file");
            exit(EXIT_FAILURE);
        }
        //for getting contents of file
        while (1) {
            bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

            if (bytes_received == -1) {
                perror("Error receiving data");
                break;
            }

            if (bytes_received == 0) {
                // End of file
                fclose(file);
                break;
            }
            //storing file
            fwrite(buffer, 1, bytes_received, file);
        }

    }

    // while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
    //     buffer[bytes_received] = '\0';

    //     // Create a file to store received data
    //     FILE *file = fopen("received_file", "ab"); // Modify filename as needed
    //     if (file == NULL) {
    //         perror("Error creating file");
    //         exit(EXIT_FAILURE);
    //     }

    //     // Write received data to the file
    //     if (fwrite(buffer, 1, bytes_received, file) != bytes_received) {
    //         perror("Error writing to file");
    //         exit(EXIT_FAILURE);
    //     }

    //     fclose(file);
    // }
    printf("folder received successfully\n");

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error in socket creation");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in binding");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        perror("Error in listen");
        exit(EXIT_FAILURE);
    }

    printf("Server started, waiting for connections...\n");

    while (1) {
        // Accept incoming connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket < 0) {
            perror("Error in accept");
            continue;
        }
        const char *folder_path = "sh_rec";  // Adjust the folder name as needed
        mkdir(folder_path, 0777);  // Create the folder
        chdir(folder_path);  // Change the working directory to the created folder
        printf("Connected to client\n");

        handle_client(client_socket, folder_path);

        printf("Disconnected from client\n");
    }

    close(server_socket);
    return 0;
}





// void receive_folder(int server_socket, const char *folder_path) {
//     struct sockaddr_in client_addr;
//     socklen_t client_addr_len = sizeof(client_addr);

//     char buffer[BUFFER_SIZE];
//     ssize_t bytes_received;

//     while (1) {
//         bytes_received = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);

//         if (bytes_received == -1) {
//             perror("Error receiving data");
//             break;
//         }

//         if (bytes_received == 0) {
//             printf("Folder received successfully.\n");
//             break;
//         }

//         // Create directories and files
//         FILE *file = fopen(buffer, "wb");
//         if (file == NULL) {
//             perror("Error creating file");
//             exit(EXIT_FAILURE);
//         }
//         //for getting contents of file
//         while (1) {
//             bytes_received = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);

//             if (bytes_received == -1) {
//                 perror("Error receiving data");
//                 break;
//             }

//             if (bytes_received == 0) {
//                 // End of file
//                 fclose(file);
//                 break;
//             }
//             //storing file
//             fwrite(buffer, 1, bytes_received, file);
//         }
//     }
    
// }

// int main() {
//     int server_socket;
//     struct sockaddr_in server_addr;

//     const char *folder_path = "sh_rec";  // Adjust the folder name as needed
//     mkdir(folder_path, 0777);  // Create the folder
//     chdir(folder_path);  // Change the working directory to the created folder

//     // Create socket
//     server_socket = socket(AF_INET, SOCK_DGRAM, 0);
//     if (server_socket == -1) {
//         perror("Error creating socket");
//         fprintf(stderr, "Error code: %d\n", errno);
//         exit(EXIT_FAILURE);
//     }

//     // Bind the socket to a specific port
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(SERVER_PORT);

//     if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
//         perror("Error binding socket");
//         exit(EXIT_FAILURE);
//     }

//     printf("Server listening on port %d\n", SERVER_PORT);

//     receive_folder(server_socket, folder_path);

//     close(server_socket);
//     return 0;
// }