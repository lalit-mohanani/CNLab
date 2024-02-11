#include <stdio.h>

#include <stdlib.h>

#include <sys/socket.h>

#include <sys/types.h>

#include <netinet/in.h>

#include <netinet/ip.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define port 8080
#define BUFFER_SIZE 100
int main()
{
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd == -1)
  {
    perror("Error creating socket");
    fprintf(stderr, "Error code: %d - %s\n", errno, strerror(errno));
    exit(EXIT_FAILURE);
  }
  else
    printf("socket creation succeeds\n");

  struct sockaddr_in servaddr;

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  servaddr.sin_addr.s_addr = inet_addr("50.50.50.50");

  int sock_bind;
  sock_bind = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

  if (sock_bind == -1)
  {
    perror("Error binding socket");
    fprintf(stderr, "Error code: %d - %s\n", errno, strerror(errno));
    exit(EXIT_FAILURE);
  }

  printf("socket binding succeeds\n");

  struct sockaddr_in sender;
  char buffer[BUFFER_SIZE];
  int addrlen = sizeof(sender);

  while (1)
  {
    int rec = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&sender, &addrlen);
    printf("Received:");
    buffer[rec] = '\0';
    if (strcmp(buffer, "exit") == 0)
      break;

    if (rec == -1)
    {
      printf("recvfrom fails\n");
      exit(0);
    }

    printf("%s\n", buffer);
  }

  close(sockfd);
  return 0;
}
