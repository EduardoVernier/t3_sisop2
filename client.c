#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

int sockfd;
pthread_t receiver;
pthread_t sender;

int connectToServer(int argc, char *argv[]);
void *sendMessages(void *arg);
void *receiveMessages(void *arg);

struct hostent *server;
int port;
char serverName[100];

int main(int argc, char *argv[]){
	if(connectToServer(argc, argv) == 1) return 0;

	pthread_create(&sender, NULL, sendMessages, NULL);
	pthread_create(&receiver, NULL, receiveMessages, NULL);

	pthread_join(sender, NULL);
	pthread_join(receiver, NULL);

	close(sockfd);
	return 0;
}

int connectToServer(int argc, char *argv[]){
	if (argc < 3){
		fprintf(stderr,"usage %s hostname #port\n", argv[0]);
		return(1);
	}
	struct sockaddr_in serv_addr;

	port = atoi(argv[2]);
	server = gethostbyname(argv[1]);

	bzero(serverName, 100);
	strcpy(serverName, argv[1]);

	if (server == NULL){
		fprintf(stderr,"ERROR, no such host\n");
		return(1);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("ERROR opening socket\n");
		return 1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
	bzero(&(serv_addr.sin_zero), 8);

	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
		printf("ERROR connecting\n");
		return 1;
	}
	return 0;
}

void *sendMessages(void *arg){
    int n;
    int logout = 0;
 	char buffer[256];

    system("clear");

    n = write(sockfd, "/name\n", strlen("/name\n"));
    if (n < 0) printf("ERROR writing to socket\n");
    printf("[New NAME]: ");
    bzero(buffer, 256);
    fgets(buffer, 256, stdin);
    n = write(sockfd, buffer, strlen(buffer));
    

 	while (!logout)
	{
		printf("[ENTER to Send]: ");
		bzero(buffer, 256);
		fgets(buffer, 256, stdin);

		n = write(sockfd, buffer, strlen(buffer));
		if (n < 0) printf("ERROR writing to socket\n");

        if(strcmp(buffer, "/logout\n") == 0)
            logout = 1;
        else if(strcmp(buffer, "/name\n") == 0){
        	printf("[New NAME]: ");
            bzero(buffer, 256);
            fgets(buffer, 256, stdin);

            n = write(sockfd, buffer, strlen(buffer));
            if (n < 0) printf("ERROR writing to socket\n");
        } else if (strcmp(buffer, "/join\n") == 0){
       		printf("[Join]: ");
            bzero(buffer, 256);
            fgets(buffer, 256, stdin);

            n = write(sockfd, buffer, strlen(buffer));
            if (n < 0) printf("ERROR writing to socket\n");        
        }

        bzero(buffer, 256);

        system("clear");
	}
	pthread_exit(0);
 }

void *receiveMessages(void *arg){
    char sysString[256];
    sprintf(sysString, "xterm -e ./screen %s %d %d", serverName, port, sockfd);
    system(sysString);

    pthread_exit(0);
 }
