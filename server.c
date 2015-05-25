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
//message_list* messageList = NULL;
room* root = NULL;
sem_t semaphore;

void *newClientConnection (void *arg);
int setUpSocket(int argc, char *argv[]);
void *newMessageDeliverer(void *arg);

int main(int argc, char *argv[]){
	setUpSocket(argc, argv);
	struct sockaddr_in cli_addr;

	//messageList = newMessageList();
	root = createRoom("Global", newMessageList(), NULL);

    sem_init(&semaphore, 0, 1);

	socklen_t clilen;
	clilen = sizeof(struct sockaddr_in);
	while(1)
	{
        int *newSock = malloc(sizeof(int));

        *newSock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		puts("Connection accepted");

        pthread_t *newClientThread = malloc(sizeof(pthread_t));
        pthread_t *newClientMessageDeliverer = malloc(sizeof(pthread_t));
        //int *newSock = malloc(sizeof(int));
        //*newSock = newsockfd;

		if(pthread_create(newClientThread, NULL, newClientConnection, (void*)newSock) < 0){
            perror("could not create thread for client");
            return 1;
        }

        if(pthread_create(newClientMessageDeliverer, NULL, newMessageDeliverer, (void*)newSock) < 0){
            perror("could not create thread for deliverer");
            return 1;
        }
	}

	close(newsockfd);
	close(sockfd);
	return 0;
}

int setUpSocket(int argc, char *argv[]){
	struct sockaddr_in serv_addr;
	int port;

	if (argc < 2){
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

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		printf("ERROR on binding");
		return 1;
	}

	listen(sockfd, MAX_CONNECTIONS);

	return 0;
}

void *newMessageDeliverer(void *arg){
    int n;
    int newsockfd = *(int*) arg;
    char msgToDeliver[256];
    message* clientLastMessage = NULL;
    room* newRoom = NULL;

    sprintf(msgToDeliver, "%s%s", root->name, "\n");
    n = write(newsockfd, msgToDeliver, strlen(msgToDeliver));
    if (n < 0) printf("ERROR writing to socket\n");
    bzero(msgToDeliver, 256);

    clientLastMessage = root->ml->end;

    if(root->ml->start != NULL){
        while(clientLastMessage->next == NULL);

        clientLastMessage = clientLastMessage->next;
    }else{
        while(root->ml->end == NULL);

        clientLastMessage = root->ml->end;
    }

	while(!(strcmp(clientLastMessage->text, "/logout\n") == 0 && clientLastMessage->id == newsockfd))
	{
        if(clientLastMessage!=NULL)
        if(strcmp(clientLastMessage->text, "/join\n") == 0 || strcmp(clientLastMessage->text, "/create\n") == 0){
            while(clientLastMessage->next == NULL);

            if(clientLastMessage->id == newsockfd){
            while(clientLastMessage->next == NULL);
                printf("\nvai procurar por: .\n", clientLastMessage->next->text);

                newRoom = getRoomByName(clientLastMessage->next->text, root);
                if(newRoom == NULL){
                    while(clientLastMessage->next->next == NULL);
                    clientLastMessage = clientLastMessage->next->next;
                }else{
                    clientLastMessage = newRoom->ml->end;
                    while(clientLastMessage->next == NULL);
                }
            }else{
                while(clientLastMessage->next->next == NULL);
                clientLastMessage = clientLastMessage->next->next;
            }
        }else
        {
            if(clientLastMessage == NULL){
                continue;
            }else{
                sprintf(msgToDeliver, "%s%s%s%s", "[", clientLastMessage->username,"]: ", clientLastMessage->text);

                n = write(newsockfd, msgToDeliver, strlen(msgToDeliver));
                if (n < 0) printf("ERROR writing to socket\n");
                bzero(msgToDeliver, 256);

                while(clientLastMessage->next == NULL);
                clientLastMessage = clientLastMessage->next;
            }
        }
    }

    //printf("\n...ending deliverer for %s on socket id %d", clientLastMessage->username, newsockfd);

    pthread_exit(0);
}

void *newClientConnection (void *arg){
    char userName[256];
    strcpy(userName, "user");
	int newsockfd = *(int*) arg;

	char buffer[256];
	bzero(buffer, 256);
	printf("New client!\n");

    int i = 0, n = 0, logout = 0, create = 0, join = 0, specialCmd = 0;
	room* myRoom = root;
	room* test = NULL;

	while (!logout)
	{
        logout = specialCmd = 0;
		n = read(newsockfd, buffer, 256);
		if(n < 0) printf("ERROR reading from socket");

        //special commands
        if(strcmp(buffer, "/logout\n") == 0){
            logout = 1;
        }else if(strcmp(buffer, "/join\n") == 0){
            join = 1;
        }else if(strcmp(buffer, "/name\n") == 0){
            n = read(newsockfd, buffer, 256);
            if(n < 0) printf("ERROR reading from socket");

            for(i=0; i<strlen(buffer); i++)
                if(buffer[i] == '\n')
                    buffer[i] = '\0';

            strcpy(userName, buffer);

            specialCmd = 1;
        }else if(strcmp(buffer, "/create\n") == 0){
            create = 1;
        }
        //end of special commands

        printf(" -> %s's MSG: %s ", userName, buffer);
        if(!specialCmd){
            message* newM = newMessage(buffer, myRoom->name, userName, newsockfd);
            addNewMessage(myRoom->ml, newM);
        }

        bzero(buffer, 256); //eliminates any vestigious in buffer to receive next message

        if(create){
            n = read(newsockfd, buffer, 256);
            if(n < 0) printf("ERROR reading from socket");

            for(i=0; i<strlen(buffer); i++)
                if(buffer[i] == '\n')
                    buffer[i] = '\0';

            test = createRoom(buffer, newMessageList(), root);
            message* newM = newMessage(buffer, myRoom->name, userName, newsockfd);
            addNewMessage(myRoom->ml, newM);
            myRoom = test;

            printf("\ncriou: %s.\n", buffer);

            create = 0;
        }

        if(join){
            n = read(newsockfd, buffer, 256);
            if(n < 0) printf("ERROR reading from socket");

            for(i=0; i<strlen(buffer); i++)
                if(buffer[i] == '\n')
                    buffer[i] = '\0';

            message* newM = newMessage(buffer, myRoom->name, userName, newsockfd);
            addNewMessage(myRoom->ml, newM);

            printf("\nvai procurar: %s.\n", buffer);
            test = getRoomByName(buffer, root);
            if(test != NULL){
                printf("\nachou!!! %s.\n", test->name);
                myRoom = test;
            }

            join = 0;
        }

        bzero(buffer, 256); //eliminates any vestigious in buffer to receive next message
	}
	if (write(newsockfd, "/disconnect\n", strlen("/disconnect\n")) < 0)
		printf("Error disconnecting client");

    pthread_exit(0);
}
