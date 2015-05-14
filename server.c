#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 4004
#define MAX_CONNECTIONS 10

void startConnection (int sockfd);

int main(int argc, char *argv[])
{
	int sockfd, newsockfd;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        printf("ERROR opening socket");
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);     
    
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
		printf("ERROR on binding");
	
	listen(sockfd, MAX_CONNECTIONS);
	
	int on = 1; // debugging purpose
	while (on)
	{
		printf("enter loop\n");
		clilen = sizeof(struct sockaddr_in);
		if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) == -1) 
			printf("ERROR on accept");
		else
			startConnection (newsockfd); //new thread for each conection
		printf("loop\n");

	}
	
	/* read from the socket */
	
	
	
	/* write in the socket */ 
	//n = write(newsockfd,"I got your message", 18);
	//if (n < 0) 
	//	printf("ERROR writing to socket");

	close(newsockfd);
	close(sockfd);
	return 0; 
}

void startConnection (int newsockfd)
{
	int n;
	char buffer[256];
	bzero(buffer, 256);
	printf("New client!\n");
	while (1) // TODO: fix later
	{
		n = read(newsockfd, buffer, 256);
		if (n < 0) 
			printf("ERROR reading from socket");
		printf("Here is the message: %s\n", buffer);
	}
}