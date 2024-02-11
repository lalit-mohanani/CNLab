#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 12345
#define MAX_CLIENTS 5
#define MAX_MSG_SIZE 1024

typedef struct {
    int client_socket;
} ThreadArgs;

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

void *handle_client(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int client_socket = args->client_socket;
    printf("Connected to client\n");

    char buffer[MAX_MSG_SIZE];
    ssize_t bytes_received;

        // Receive data from client
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    buffer[bytes_received] = '\0';
    printf("Expression received from client: %s\n", buffer);
    int result = calculate_postfix(buffer);
    // Send result back to client
    sleep(10);
    char response[MAX_MSG_SIZE];
    
    snprintf(response, sizeof(response), "%d", result);
    send(client_socket, response, strlen(response), 0);
    printf("Result sent to client: %s\n", response);
    printf("Disconnected from client\n");

    close(client_socket);
    free(args);
    pthread_exit(NULL);
}


int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t tid;
    ThreadArgs *args;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 12345
    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    //     perror("setsockopt");
    //     exit(EXIT_FAILURE);
    // }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 12345
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        if (args == NULL) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        args->client_socket = new_socket;

        if (pthread_create(&tid, NULL, handle_client, (void *)args) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
