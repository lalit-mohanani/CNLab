#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SIZE 1024

void send_file_data(FILE* fp, int sockfd, struct sockaddr_in addr)
{
  int n,s=0;
  char buffer[SIZE];

  // Sending the data
  while (1)
  {
    if(fgets(buffer, SIZE, fp)==NULL) break;
    printf("[SENDING] Data: %s", buffer);
    s++;
    n = sendto(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));
    if (n == -1)
    {
      perror("[ERROR] sending data to the server.");
      exit(1);
    }
    bzero(buffer, SIZE);
    // usleep(1000);
  }
  printf("%d\n",s);
  // Sending the 'END'
  sleep(1);
  bzero(buffer, SIZE);
  strcpy(buffer, "END");

  sendto(sockfd, buffer, SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));

  fclose(fp);
}

int main(void)
{

  // Defining the IP and Port
  char *ip = "10.10.64.59";
  int port = 8080;

  // Defining variables
  int server_sockfd;
  struct sockaddr_in server_addr;
  char *filename = "file2.txt";
  FILE *fp = fopen(filename, "r");

  // Creating a UDP socket
  server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_sockfd < 0)
  {
    perror("[ERROR] socket error");
    exit(1);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(ip);

  // Reading the text file
  if (fp == NULL)
  {
    perror("[ERROR] reading the file");
    exit(1);
  }

  // Sending the file data to the server
  send_file_data(fp, server_sockfd, server_addr);

  printf("[SUCCESS] Data transfer complete.\n");
  printf("[CLOSING] Disconnecting from the server.\n");

  close(server_sockfd);

  return 0;
}
