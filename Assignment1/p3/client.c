#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SIZE 1024

void send_file_data(FILE* fp, int sockfd, struct sockaddr_in addr)
{
  int n;
  char client_buffer[SIZE];

  // Sending the data
  while (fgets(client_buffer, SIZE, fp) != NULL)
  {
    printf("[SENDING] Data: %s", client_buffer);

    n = sendto(sockfd, client_buffer, SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (n == -1)
    {
      perror("[ERROR] sending data to the server.");
      exit(1);
    }
    bzero(client_buffer, SIZE);
  }
  sleep(1);
  // Sending the 'END'
  strcpy(client_buffer, "END");
  sendto(sockfd, client_buffer, SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));

  fclose(fp);
}

int main(void)
{

  // Defining the IP and Port
  char *ip = "127.0.0.1";
  const int port = 8080;

  // Defining variables
  int sockfd;
  struct sockaddr_in server_addr;
  char *filename = "client_text.txt";
  FILE *fp = fopen(filename, "r");

  // Creating a UDP socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    perror("[ERROR] socket error");
    exit(1);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  // Reading the text file
  if (fp == NULL)
  {
    perror("[ERROR] reading the file");
    exit(1);
  }

  // Sending the file data to the server
  send_file_data(fp, sockfd, server_addr);

  printf("[SUCCESS] Data transfer complete.\n");
  printf("[CLOSING] Disconnecting from the server.\n");

  close(sockfd);

  return 0;
}
