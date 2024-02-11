#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_MSG_SIZE 1024
#define SERVER_PORT 12345

void handle_client(int client_socket) {
    char buffer[MAX_MSG_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_received] = '\0';

        // Create a file to store received data
        FILE *file = fopen("received_folder.zip", "ab"); // Modify filename as needed
        if (file == NULL) {
            perror("Error creating file");
            exit(EXIT_FAILURE);
        }

        // Write received data to the file
        if (fwrite(buffer, 1, bytes_received, file) != bytes_received) {
            perror("Error writing to file");
            exit(EXIT_FAILURE);
        }

        fclose(file);
    }

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

        printf("Connected to client\n");

        handle_client(client_socket);

        printf("Disconnected from client\n");
    }

    close(server_socket);
    return 0;
}
