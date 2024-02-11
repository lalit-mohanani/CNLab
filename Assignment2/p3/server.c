#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h> // Include pthread library for threading

#define PORT 8080
#define BUFFER_SIZE 1024

void *handle_client(void *socket_desc) {
    int client_socket = *(int *)socket_desc;
    char buffer[BUFFER_SIZE] = {0};
    char file_name[BUFFER_SIZE];;

    ssize_t bytes_received;
    if ((bytes_received = recv(client_socket, file_name, BUFFER_SIZE, 0)) < 0) {
        perror("Error receiving file name.");
        exit(EXIT_FAILURE);
    }
    // file_name[bytes_received] = 'r';
    file_name[bytes_received] = '\0';
    printf("File name received: %s\n", file_name);
    FILE *received_file = fopen(file_name, "wb");
    if (received_file == NULL) {
        perror("Error opening file.");
        exit(EXIT_FAILURE);
    }

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, sizeof(char), bytes_received, received_file);
    }
    if (bytes_received < 0) {
        perror("Error receiving file.");
        exit(EXIT_FAILURE);
    }
    sleep(10);
    printf("File received successfully.\n");

    fclose(received_file);
    close(client_socket);
    free(socket_desc);
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t thread_id;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
 
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen))) {
        int *new_socket_ptr = malloc(sizeof(int));
        *new_socket_ptr = new_socket;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)new_socket_ptr) < 0) {
            perror("could not create thread");
            exit(EXIT_FAILURE);
        }
    }

    if (new_socket < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    close(server_fd);
    return 0;
}
