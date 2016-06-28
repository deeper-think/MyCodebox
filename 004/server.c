/* a sample udp socket server
 * gcc server.c -o server */


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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(80);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    int n;
    char recvline[1024];

    recvfrom(sockfd, recvline, 1024, 0, NULL, NULL);

    printf("%s\n", recvline);

    close(sockfd);
}
