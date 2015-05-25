#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include "message_list.c"

#define MAX_CONNECTIONS 10

int sockfd, newsockfd;
pthread_t runner;
message_list* messageList = NULL;
sem_t semaphore;

void *newClientConnection (void *arg);
int setUpSocket(int argc, char *argv[]);
void *newMessageDeliverer(void *arg);

int main(int argc, char *argv[])
{
	setUpSocket(argc, argv);
	struct sockaddr_in cli_addr;

	messageList = newMessageList();

    sem_init(&semaphore, 0, 1);

	socklen_t clilen;
	clilen = sizeof(struct sockaddr_in);
	while(1)
	{
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		puts("Connection accepted");

        pthread_t newClientThread;
        pthread_t newClientMessageDeliverer;
        int *newSock = malloc(sizeof(int));
        *newSock = newsockfd;

		if(pthread_create(&newClientThread , NULL ,  newClientConnection , (void*)newSock) < 0){
            perror("could not create thread");
            return 1;
        }

        if(pthread_create(&newClientMessageDeliverer , NULL ,  newMessageDeliverer , (void*)newSock) < 0){
            perror("could not create thread");
            return 1;
        }
	}

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

void *newMessageDeliverer(void *arg){
    int newsockfd = *(int*) arg;
    message* clientLastMessage = NULL;

    while(messageList->end == NULL);

    clientLastMessage = messageList->end;

	while(!(strcmp(clientLastMessage->text, "/logout\n") == 0 && clientLastMessage->id == newsockfd))
	{
        if(clientLastMessage == NULL){
            continue;
        }else{
            int n;
            char msgToDeliver[256];
            sprintf(msgToDeliver, "%s%s%s%s", "[", clientLastMessage->username,"]: ", clientLastMessage->text);

            n = write(newsockfd, msgToDeliver, strlen(msgToDeliver));
            if (n < 0) printf("ERROR writing to socket\n");

            while(clientLastMessage->next == NULL);
            clientLastMessage = clientLastMessage->next;
        }
    }
}

void *newClientConnection (void *arg)
{
    char userName[256];
    strcpy(userName, "user");
	int newsockfd = *(int*) arg;

	char buffer[256];
	bzero(buffer, 256);
	printf("New client!\n");

    int n;
	int logout;
	int specialCmd;
	while (!logout)
	{
        logout = 0;
		specialCmd = 0;
		n = read(newsockfd, buffer, 256);
		if(n < 0) printf("ERROR reading from socket");

        printf("\n...fez read...\n");

        //special commands
        if(strcmp(buffer, "/logout\n") == 0){
            logout = 1;
            specialCmd = 1;
        } else if(strcmp(buffer, "/join\n") == 0){
            n = read(newsockfd, buffer, 256);
            if(n < 0) printf("ERROR reading from socket");

            //logar usuario na sala == trocar ponteiro q ele usa para receber mensagens (na função New Message Deliverer)
            specialCmd = 1;
        } else if(strcmp(buffer, "/leave\n") == 0){

            //anular o ponteiro do usuário para receber mensagens
            specialCmd = 1;
        } else if(strcmp(buffer, "/name\n") == 0){
            n = read(newsockfd, buffer, 256);
            if(n < 0) printf("ERROR reading from socket");

            int i;
            for(i=0; i<strlen(buffer); i++)
                if(buffer[i] == '\n')
                    buffer[i] = '\0';

            strcpy(userName, buffer);

            specialCmd = 1;
        } else if(strcmp(buffer, "/create\n") == 0){
            specialCmd = 1;
        }
        //end of special commands

        printf("%s's MSG: %s", userName, buffer);
        if(!specialCmd){
            // provisory - room will be sent with every message (parsing needed)
            char _room [10] = "room";
            message* newM = newMessage(buffer, _room, userName, newsockfd);
            addNewMessage(messageList, newM);
            printMessages(messageList);
            printf("\n...voltou do print...\n");
        }else{
            specialCmd = 0;
        }

        bzero(buffer, 256); //eliminates any vestigious in buffer to receive next message
	}
	if (write(newsockfd, "/disconnect\n", strlen("/disconnect\n")) < 0)
		printf("Error disconnecting client");
}
