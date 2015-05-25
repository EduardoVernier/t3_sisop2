#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include "message_list.c"
int sockfd;
pthread_t receiver;
pthread_t sender;
message_list* messageList;
char username[256] = "Anon";
char typingBuffer[256] = "";

int connectToServer(int argc, char *argv[]);
void *sendMessages(void *arg);
void *receiveMessages(void *arg);
void printLog();

int main(int argc, char *argv[])
{
	messageList = newMessageList();
	
	if (connectToServer(argc, argv) == 1)
		return 0;
	
	pthread_create(&sender, NULL, sendMessages, NULL);
	pthread_create(&receiver, NULL, receiveMessages, NULL);
	
	pthread_join(sender, NULL);
	pthread_join(receiver, NULL);
	
	close(sockfd);
	return 0;
}

int connectToServer(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr,"usage %s hostname #port\n", argv[0]);
		return(1);
	}	
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int port;
	
	port = atoi(argv[2]);
	server = gethostbyname(argv[1]);
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		return(1);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		printf("ERROR opening socket\n");
		return 1;
	}
	
	serv_addr.sin_family = AF_INET;     
	serv_addr.sin_port = htons(port);    
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(serv_addr.sin_zero), 8);     

	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
	{
		printf("ERROR connecting\n");
		return 1;
	}
	
	printf ("Type your username: ");
	bzero(username, 256);
	fgets(username, 256, stdin);
	username[strlen(username) -1] = '\0';
	return 0;
}
void *sendMessages(void *arg)
{
	while (strcmp(typingBuffer, "/logout\n") != 0)
	{
		bzero(typingBuffer, 256);
		fgets(typingBuffer, 256, stdin);
		
		/*
		char c;
		do {
			c = getchar();
			strcat (typingBuffer, &c);
		} while (c != '\n');
		*/
		// didn't work
		
		/* write in the socket */
		int n;
		char str[256];
		strcpy(str, username);
		strcat(str, ": ");
		strcat(str, typingBuffer);
		n = write(sockfd, str, strlen(str));
		bzero(str, 256);
		if (n < 0) 
			printf("ERROR writing to socket\n");
		//printf("\n%s: ", username);
		
	}
}

void *receiveMessages(void *arg)
{
	char buffer[256];
	int n;

	while (strcmp(buffer, "/disconnect\n") != 0)
	{
		bzero(buffer,256);
		n = read(sockfd, buffer, 256);
		if (n < 0) 
			printf("ERROR reading from socket\n");
		printf("%s\n", buffer);
		message* newM = newMessage(buffer, "room");
		addNewMessage(messageList, newM);
		printLog();
	}
}

void printLog()
{
	system("clear");
	printMessages(messageList);
	fprintf(stderr,"\nType your messages here: ");
	fprintf(stderr,"%s",typingBuffer);
	printf("%s",typingBuffer);
}



