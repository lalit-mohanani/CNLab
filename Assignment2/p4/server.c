#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <dirent.h>

#define PORT 8080
#define MAX_FILENAME_LEN 256
#define MAX_BUFFER_SIZE 1024
#define IP "127.0.0.1"
#define MAX_CONN 10

void handle_connection(int client_socket)
{
    char filename[MAX_FILENAME_LEN];
    ssize_t bytes_received;

    // Receive filename
    if ((bytes_received = recv(client_socket, filename, MAX_FILENAME_LEN, 0)) == -1)
    {
        perror("Receive failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Received file: %s\n", filename);

    // Open file
    FILE *file = fopen(filename, "wb");

    // Receive file data and write to file
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0)) > 0)
    {
        fwrite(buffer, 1, bytes_read, file);
    }

    // Close file and connection
    fclose(file);
    close(client_socket);
    printf("File received successfully: %s\n", filename);

    exit(EXIT_SUCCESS);
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address parameters
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to address
    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Listen for connections
    listen(server_socket, MAX_CONN);

    printf("Server listening on port %d...\n", PORT);

    // Accept connections and handle them in separate processes
    const char *folder_path = "sh_rec";  // Adjust the folder name as needed
    mkdir(folder_path, 0777);  // Create the folder
    chdir(folder_path);  
    while (1)
    {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) == -1)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pid_t child_pid = fork();
        
        if (child_pid == 0)
        {
            // Child process
            handle_connection(client_socket);
        }
    }

    close(server_socket);
    return 0;
}