#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "message_list.c"
#include "client_list.c"

#define MAX_CONNECTIONS 10

int sockfd, newsockfd;
pthread_t runner;
message_list* messageList;
client_list* clientList;

void *newClientConnection (void *arg);
int setUpSocket(int argc, char *argv[]);
void sendMessage(client_list *cl, message* m);

int main(int argc, char *argv[])
{
	setUpSocket(argc, argv);
	struct sockaddr_in cli_addr;
	
	messageList = newMessageList();
	clientList = newClientList();

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
	client *c = newClient ("John", newsockfd);
	addNewClient(clientList, c);
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
		
		// provisory - room will be sent with every message (parsing needed)
		char _room [10] = "futebol";
		message* newM = newMessage(buffer, _room);
		addNewMessage(messageList, newM);
		printMessages(messageList);
		sendMessage(clientList, newM);
	}
	if (write(newsockfd, "/disconnect\n", 12) < 0)
		printf("Error disconnecting client");
}

void sendMessage(client_list *cl, message* m)
{
	client *c = cl->start;
	while (1)
	{
		if (c == NULL) break;
		write(c->sock, m->text, strlen(m->text));
		//printf("wrote> %s\n", m->text);
		if (c->next == NULL) break;
		c = c->next;
	}
}
