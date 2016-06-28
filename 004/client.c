/* a sample udp socket client code. 
 * gcc  client.c -o client*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(80);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    char sendline[100];
    sprintf(sendline, "Hello, world!");

    sendto(sockfd, sendline, strlen(sendline), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

    close(sockfd);

    return 0;
}
