#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_MSG_SIZE 1024

int calculate_postfix(char *expression) {
    int stack[MAX_MSG_SIZE];
    int top = -1;

    char *token = strtok(expression, " ");
    while (token != NULL) {
        if (isdigit(*token)) {
            stack[++top] = atoi(token);
        } else {
            int operand2 = stack[top--];
            int operand1 = stack[top--];
            switch (*token) {
                case '+':
                    stack[++top] = operand1 + operand2;
                    break;
                case '-':
                    stack[++top] = operand1 - operand2;
                    break;
                case '*':
                    stack[++top] = operand1 * operand2;
                    break;
                case '/':
                    stack[++top] = operand1 / operand2;
                    break;
            }
        }
        token = strtok(NULL, " ");
    }
    return stack[top];
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
    server_addr.sin_port = htons(12345);
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

        char buffer[MAX_MSG_SIZE];
        ssize_t bytes_received;

        // Receive data from client
        while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
            buffer[bytes_received] = '\0';
            int result = calculate_postfix(buffer);

            // Send result back to client
            char response[MAX_MSG_SIZE];
            
            snprintf(response, sizeof(response), "%d", result);
            send(client_socket, response, strlen(response), 0);
        }

        printf("Disconnected from client\n");
        close(client_socket);
    }

    close(server_socket);
    return 0;
}
