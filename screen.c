#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

 int main(int argc, char *argv[])
 {
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int port;
	int sockfd;

    port = atoi(argv[2]);
	server = gethostbyname(argv[1]);
	sockfd = atoi(argv[3]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(serv_addr.sin_zero), 8);

	connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));

 	char buffer[256];
	int n;
	int logout = 0;
 	while (!logout)
	{
        bzero(buffer,256);
        n = read(sockfd, buffer, 256);
        if (n < 0)
            printf("ERROR reading from socket\n");
        printf("%s", buffer);

        if(strcmp(buffer, "/disconnect\n") == 0)
            logout = 1;
        bzero(buffer,256);
 	}

 	close(sockfd);
}
