#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET "\033[0m"

int main(int argc, char *argv[]){
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

    printf(KRED "         ***   WELCOME!   ***\n\n" RESET);
    printf(KMAG "  [1]   " KGRN "First, choose your name...\n" RESET);
    printf(KMAG "  [2]   " KYEL "Second, choose a chat room...\n" RESET);
    printf(KMAG "  [3]   " KCYN "Third, let's chat!\n" RESET);

    printf(KRED "\nSpecial Commmands:\n" RESET);
    printf(KMAG "  /" KBLU "logout " KWHT ": to exit chat\n" RESET);
    printf(KMAG "  /" KBLU "name   " KWHT ": change username\n" RESET);
    printf(KMAG "  /" KBLU "join   " KWHT ": enter a different chat room\n");
    printf(KMAG "   " KBLU "       " KWHT "  (creates a new one if it doesn't exist)\n\n" RESET);


 	while (!logout)
	{
        bzero(buffer,256);
        n = read(sockfd, buffer, 256);
        if (n < 0) printf("ERROR reading from socket\n");
        printf("%s" RESET, buffer);

        if(strcmp(buffer, "/disconnect\n") == 0)
            logout = 1;
        bzero(buffer,256);
 	}

 	close(sockfd);
}
