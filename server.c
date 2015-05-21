#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_CONNECTIONS 10

int sockfd, newsockfd;
pthread_t runner;

void *newClientConnection (void *arg);
int setUpSocket(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	setUpSocket(argc, argv);
	struct sockaddr_in cli_addr;
	
	
	socklen_t clilen;
	clilen = sizeof(struct sockaddr_in);
	while (newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen))
	{
		puts("Connection accepted");
         
        pthread_t newClientThread;
        int *newSock = malloc(sizeof(int));
        *newSock = newsockfd;
		
		if( pthread_create( &newClientThread , NULL ,  newClientConnection , (void*)newSock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        
			//startConnection (); //new thread for each conection


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

int setUpSocket(int argc, char *argv[])
{
	struct sockaddr_in serv_addr;
	int port;	

	if (argc < 2)
	{
		fprintf(stderr,"usage %s #port\n", argv[0]);
		return(1);
	}
	
	port = atoi(argv[1]);
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        printf("ERROR opening socket");
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);     
    
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
	{
		printf("ERROR on binding");
		return 1;	
	}
		
	listen(sockfd, MAX_CONNECTIONS);
}

void *newClientConnection (void *arg)
{
	int newsockfd = *(int*) arg;
	
	char buffer[256];
	bzero(buffer, 256);
	printf("New client!\n");
	while (strcmp(buffer, "/logout\n"))
	{
		int n;
		n = read(newsockfd, buffer, 256);
		if (n < 0) 
			printf("ERROR reading from socket");
		printf("Here is the message: %s", buffer);
	}
	if (write(newsockfd, "/disconnect\n", 12) < 0)
		printf("Error disconnecting client");
}
