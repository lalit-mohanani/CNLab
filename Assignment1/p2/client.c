#include<stdio.h>

#include<stdlib.h>

#include<sys/socket.h>

#include<sys/types.h>

#include<netinet/in.h>

#include<netinet/ip.h>
#include<arpa/inet.h>
#include<string.h>

#include<unistd.h>
#define port 3000
 
int main() {
  // create a socket
  char *ip = "10.10.150.99";
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd == -1) {
    perror("Error creating socket");
        exit(EXIT_FAILURE);
  } else
    printf("\nsocket creation succeeds");
  // for(int i=0;i<5;i++){
  struct sockaddr_in sender;
  
  sender.sin_family = AF_INET;
  sender.sin_port = htons(port+i);
  sender.sin_addr.s_addr = inet_addr(ip);

  char str[15];

  int addrlen = sizeof(sender);
  
    printf("Enter message: ");
    fgets(str, sizeof(str), stdin);

    int send = sendto(sockfd, str, strlen(str), 0, (struct sockaddr * ) & sender, addrlen);
    // printf("send= %d\n",send);
    if (send==-1) {
      //udp does not check if packet has been received or not
      printf("Error in sending msg");
        exit(EXIT_FAILURE);
    }

    if (strcmp(str, "exit") == 0)
      break;

    sleep(2);
    
  }
  close(sockfd);
  return 0;
}