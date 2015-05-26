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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int sockfd, newsockfd;
message_list* messageList;
client_list* clientList;

	
void *newClientConnection (void *arg);
int setUpSocket(int argc, char *argv[]);
void sendMessage(client_list *cl, message* m);
char* colorByName(char firstLetter);

/* Terminal colors related defines */
#define KWHT  "\x1B[37m"
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"

int main(int argc, char *argv[])
{
	setUpSocket(argc, argv);
	struct sockaddr_in cli_addr;

	messageList = newMessageList();
	clientList = newClientList();



	socklen_t clilen;
	clilen = sizeof(struct sockaddr_in);
	while(1) // TODO: 
	{
        int *newSock = malloc(sizeof(int));
        *newSock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		puts("Connection accepted");
		
		/* Launch a new thread for a new client with it's data socket as argument */
        pthread_t *newClientThread = malloc(sizeof(pthread_t));
		if(pthread_create(newClientThread, NULL, newClientConnection, (void*)newSock) < 0){
            perror("Could not create thread for client");
            return 1;
        }
	}

	close(newsockfd);
	close(sockfd);
	return 0;
}

/* Sets up server socket */
int setUpSocket(int argc, char *argv[])
{
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

/* Handles all actions regarding a client */
void *newClientConnection (void *arg)
{
	int newsockfd = *(int*) arg;
    char userName[256];
    char room[256] = "Global"; // Starts in Global room
    strcpy(userName, "user");

	char buffer[256];
	bzero(buffer, 256);
	printf("New client!\n");

    int i = 0, n = 0, logout = 0, specialCmd = 0;

	while (!logout)
	{
        logout = specialCmd = 0;
		n = read(newsockfd, buffer, 256);
		if(n < 0) printf("ERROR reading from socket");

        /* Special commands */
        if(strcmp(buffer, "/logout\n") == 0)
            logout = 1;
        else if(strcmp(buffer, "/join\n") == 0) // If a client wants to join a room
        {
            n = read(newsockfd, buffer, 256);
            if(n < 0) printf("ERROR reading from socket");
            for(i=0; i<strlen(buffer); i++)
                if(buffer[i] == '\n')
                    buffer[i] = '\0';
            strcpy(room, buffer); 
            setRoom(clientList, userName, room); 
            bzero(buffer, 256);
            specialCmd = 1;
        }
        else if(strcmp(buffer, "/name\n") == 0)
        {
            n = read(newsockfd, buffer, 256);
            if(n < 0) printf("ERROR reading from socket");

            for(i=0; i<strlen(buffer); i++)
                if(buffer[i] == '\n')
                    buffer[i] = '\0';

            strcpy(userName, buffer);
            client *c = newClient(userName, newsockfd, room);
			addNewClient(clientList, c);
            specialCmd = 1;
        }

        printf(" -> %s: %s ", userName, buffer);
        if(!specialCmd){
            pthread_mutex_lock(&mutex);
		        message* newM = newMessage(buffer, room, userName, newsockfd);
		        addNewMessage(messageList, newM);
		        sendMessage(clientList, newM);
            pthread_mutex_unlock(&mutex);        
        }

        bzero(buffer, 256); // Eliminates any vestigious in buffer to receive next message

	}
	if (write(newsockfd, "/disconnect\n", strlen("/disconnect\n")) < 0)
		printf("Error disconnecting client");

    pthread_exit(0);
}

/* Sends message to all clients in the same room */
void sendMessage(client_list *cl, message* m)
{
	client *c = cl->start;
	while(1)
	{
		if (c == NULL) break;
		if (strcmp(c->room, m->room)==0)
		{
			char msgToDeliver[256];
			char msgColor[3];
			strcpy(msgColor, colorByName(m->username[0]));
			sprintf(msgToDeliver, KWHT "#%-10s %s@%-10s" KWHT " : %s%s",
                    m->room, msgColor, m->username, msgColor, m->text);
			write(c->sock, msgToDeliver, strlen(msgToDeliver));
		}
		if (c->next == NULL) break;
		c = c->next;
	}
}

/* Chooses user color in terminal */
char* colorByName(char firstLetter)
{
    firstLetter = toupper(firstLetter);
    if(firstLetter >= 'A' && firstLetter <= 'D'){
        return KRED;
    }else if(firstLetter >= 'E' && firstLetter <= 'H'){
        return KGRN;
    }else if(firstLetter >= 'I' && firstLetter <= 'L'){
        return KBLU;
    }else if(firstLetter >= 'M' && firstLetter <= 'P'){
        return KYEL;
    }else if(firstLetter >= 'Q' && firstLetter <= 'T'){
        return KMAG;
    }else if(firstLetter >= 'U' && firstLetter <= 'Z'){
        return KCYN;
    }else{
        return RESET;
    }
}
